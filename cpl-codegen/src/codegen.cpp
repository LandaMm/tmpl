#include <format>

#include "cpl-codegen/codegen.h"
#include "cpl-codegen/dumper.h"

namespace Codegen
{

class GeneratorAllocatorHandler : public AllocatorHandler
{
public:
	GeneratorAllocatorHandler(FileStreamWriter* output) : m_output(output) { }
private:
	void Write(const String& what)
	{
		m_output->Write(what);
	}

	void WriteLn(const String& what)
	{
		m_output->Write(what + "\n");
	}
public:
	void OnNewStackSlot(const StackSlot& slot) override {
		WriteLn("\t;; new stack slot");
		WriteLn(std::format("\tsub rsp, {}", slot.size));
	}
	void OnSpill(const Reg& from, const StackSlot& to) override {
		WriteLn("\t;; register spill");
		WriteLn(std::format("\tmov [rbp-{}], {}", to.offset, from.name.c_str()));
		WriteLn(std::format("\txor {}, {}", from.name.c_str(), from.name.c_str()));
	}
	void OnFreeReg(const Reg& reg) override {
		WriteLn(std::format("\t;; free {} reg", reg.name.c_str()));
		WriteLn(std::format("\txor {}, {}", reg.name.c_str(), reg.name.c_str()));
	}
	void OnMove(const std::variant<Reg, StackSlot>& from, const std::variant<Reg, StackSlot>& to) override {
		WriteLn("\t;; moving");
		Write("\tmov ");
		if (std::holds_alternative<Reg>(to))
		{
			Write(std::get<Reg>(to).name.c_str());
		}
		else
		{
			Write(std::format("[rbp-{}]", std::get<StackSlot>(to).offset));
		}
		Write(", ");
		if (std::holds_alternative<Reg>(from))
		{
			Write(std::get<Reg>(from).name.c_str());
		}
		else
		{
			Write(std::format("[rbp-{}]", std::get<StackSlot>(from).offset));
		}
		WriteLn("");
	}
	void OnImmediateStore(const std::variant<Reg, StackSlot>& to, const IRGenerate::ImmediateValue* value) override {
		WriteLn("\t;; immediate store");
		if (std::holds_alternative<Reg>(to))
		{
			auto& reg = std::get<Reg>(to);
			WriteLn(std::format("\tmov {}, {}", reg.name.c_str(), value->ImmValue()));
		}
		else // StackSlot
		{
			auto& stack = std::get<StackSlot>(to);
			WriteLn(std::format("\tmov [rbp-{}], {}", stack.offset, value->ImmValue()));
		}
	}
private:
	FileStreamWriter* m_output;
};

Generator::Generator(FileStreamWriter* output, const IRGenerate::IR* ir)
		: m_ir(ir), m_output(output)
{
	using namespace IRGenerate;

	GeneratorAllocatorHandler* handler = new GeneratorAllocatorHandler(m_output);
	RegisterDistributor* distributor = new RegisterDistributor({
		Reg{"al", 8},
		Reg{"ax", 16},
		Reg{"eax", 32},
		Reg{"rax", 64},

		Reg{"cl", 8},
		Reg{"cx", 16},
		Reg{"ecx", 32},
		Reg{"rcx", 64},

		Reg{"dh", 8},
		Reg{"dx", 16},
		Reg{"edx", 32},
		Reg{"rdx", 64},

		Reg{"r9b", 8},
		Reg{"r9w", 16},
		Reg{"r9d", 32},
		Reg{"r9", 64},

		Reg{"r8b", 8},
		Reg{"r8w", 16},
		Reg{"r8d", 32},
		Reg{"r8", 64},
	});

	m_allocator = new BasicSlotAllocator(distributor, handler);
}

void Generator::Generate()
{
    using namespace IRGenerate;
    
	WriteLn("; Symbols:\n");
	for (auto& [_, symbol] : m_ir->Symbols())
	{
		if (symbol->Origin() != SymbolOrigin::FOREIGN)
		{
			WriteLn(std::format("global {}", symbol->Name().c_str()));
		}
		else
		{
			WriteLn(std::format("extern {}", symbol->Name().c_str()));
		}
	}

	WriteLn("\nsection .text\n");
	const Scope* globalScope = m_ir->Scopes().back();
	for (auto& [_, function] : globalScope->Functions())
	{
		WriteLn(std::format("{}:", function->Name().c_str()));
		WriteLn("\tpush rbp");
		WriteLn("\tmov rbp, rsp");

		GenerateBasicBlock(function->Body());

		WriteLn("\tmov rsp, rbp");
		WriteLn("\tpop rbp");
		WriteLn("\tret");
		WriteLn("");
	}

	GenerateData();
}

void Generator::GenerateBasicBlock(const IRGenerate::BasicBlock* block)
{
	WriteLn(std::format(".L{}:", block->Id()));
	for (auto instr : block->Body())
	{
		GenerateInstr(instr);
		// WriteLn(std::format("\t{}", static_cast<int>(instr->Type())));
	}
}

void Generator::GenerateInstr(const IRGenerate::Instr* instr)
{
	using namespace IRGenerate;

	switch (instr->Type())
	{
	case InstrOp::ALLOCA:
		{
			auto alloc = instr->As<AllocaInstr>();
			const LocalValue* dest = alloc->Dest()->As<LocalValue>();
			assert(dest);
			m_allocator->StoreLocal(dest);
		}
		break;
	case InstrOp::STORE:
		{
			auto store = instr->As<StoreInstr>();
			const LocalValue* dest = store->Dest()->As<LocalValue>();
			assert(dest);
			const StackSlot& slot = m_allocator->GetLocal(dest);
			const Reg& src = m_allocator->LoadValueInReg(store->Src());
			WriteLn("\t;; store");
			WriteLn(std::format("\tmov [rbp-{}], {}", slot.offset, src.name.c_str()));
			m_allocator->FreeReg(src);
		}
		break;
	case InstrOp::CALL:
		{
			auto call = instr->As<CallInstr>();
			assert(call->Args().Size() <= 4 && "only up to 4 arguments are supported for function call");
			// Array<String> parameterRegs = { "ecx", "edx", "r8d", "r9d" };
			Array<String> parameterRegs = { "rcx", "edx", "r8d", "r9d" };
			Array<Reg> argRegs;
			WriteLn("\t;; funcall");
			WriteLn("\t;; arguments");
			for (size_t i = 0; i < call->Args().Size(); ++i)
			{
				// FIXME: load arguments into specific registers following the calling convention
				argRegs.Push(m_allocator->LoadValueInReg(call->Args()[i]));
			}

			WriteLn("\t;; call");
			WriteLn(std::format("\tcall {}", call->CallSymbol()->Name().c_str()));

			// free argument registers
			for (auto &reg : argRegs)
			{
				m_allocator->FreeReg(reg);
			}
		}
		break;
	case InstrOp::LOAD:
		{
			auto load = instr->As<LoadInstr>();
			assert(load->Src()->Kind() == ValueKind::LOCAL);
			auto src = load->Src()->As<LocalValue>();
			assert(src);
			StackSlot srcSlot = m_allocator->GetLocal(src);
			Reg dstReg = m_allocator->LoadValueInReg(load->Dest());
			WriteLn("\t;; load");
			WriteLn(std::format("\tmov {}, [rbp-{}]", dstReg.name.c_str(), srcSlot.offset));
		}
		break;
	case InstrOp::PTR:
		{
			auto ptr = instr->As<PtrInstr>();
			WriteLn("\t;; ptr");
			const Value* src = ptr->Src();
			Reg dstReg = m_allocator->LoadValueInReg(ptr->Dest());
			switch (src->Kind())
			{
			case ValueKind::GLOBAL:
			{
				auto global = src->As<GlobalValue>();
				WriteLn(std::format("\tlea {}, [rel {}]", dstReg.name.c_str(), global->Name().c_str()));
			}
			break;
			case ValueKind::LOCAL:
			{
				auto local = src->As<LocalValue>();
				StackSlot slot = m_allocator->GetLocal(local);
				WriteLn(std::format("\tlea {}, [rbp-{}]", dstReg.name.c_str(), slot.offset));
			}
			break;
			case ValueKind::UNKNOWN:
			default:
				// TODO: better error
				assert(false && "unsupported value kind for address loading");
				break;
			}
		}
		break;
	case InstrOp::NONE:
	default:
		// TODO: better error
		WriteLn(std::format("\tTODO: instr op {}", static_cast<int>(instr->Type())));
		break;
	}
}

void Generator::GenerateData()
{
	using namespace IRGenerate;

	WriteLn("\nsection '.data' writeable\n");
	for (auto& [_, symbol] : m_ir->Symbols())
	{
		if (auto localVariable = dynamic_cast<const Symbols::LocalVariable*>(symbol))
		{
			Write(symbol->Name() + ": ");
			
			auto value = localVariable->InitialValue();

			IRGenerate::TypeLayout layout = IRGenerate::TypeResolver::ResolveTypeSize(localVariable->Typ());

			int step = 1;

			step = layout.size / 8;

			switch (value->Kind())
			{
			case ValueKind::IMMEDIATE:
			{
				auto imm = value->As<ImmediateValue>();

				Write(std::format("db {}", std::to_string(imm->ImmValue())));
			}
			break;
			case ValueKind::GLOBAL:
			{
				auto global = value->As<GlobalValue>();
				WriteLn(std::format("equ {}", global->Name().c_str()));
				/**
				const Array<Byte>& data = global->Data();
				for (size_t i = 0; i < data.Size() - step + 1; i+=step)
				{
					Write("\tdb ");
					const Byte* row = data.Data() + i;
					for (size_t j = 0; j < step; ++j)
					{
						if (j > 0) Write(" ");
						Write(std::format("{:#04x}", row[j]));
					}
					WriteLn("");
				}
				*/
			}
			break;
			case ValueKind::UNKNOWN:
			default:
				// TODO: better error
				assert(false && "unsupported value kind for data definition");
			}

			WriteLn("");
		}
	}

	for (auto& [_, stringLiteral] : m_ir->StringLiterals())
	{
		Write(std::format("@str.{}: db ", stringLiteral.id));

		for (size_t i = 0; i < stringLiteral.data.Size(); ++i)
		{
			char byte = stringLiteral.data.Data()[i];
			if (i > 0) Write(", ");
			Write(std::format("{:#04x}", byte));
		}
	}
}

void Generator::Write(const String& what)
{
	m_output->Write(what);
}

void Generator::WriteLn(const String& what)
{
	m_output->Write(what + "\n");
}

} // namespace Codegen

