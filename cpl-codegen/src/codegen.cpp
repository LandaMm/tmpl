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
	void OnSpill(const RegSlot& from, const StackSlot& to) override {
		WriteLn("\t;; register spill");
		WriteLn(std::format("\tmov [rbp-{}], {}", to.offset, from.name.c_str()));
		WriteLn(std::format("\txor {}, {}", from.name.c_str(), from.name.c_str()));
	}
	void OnFreeReg(const RegSlot& reg) override {
		WriteLn(std::format("\t;; free {} reg", reg.name.c_str()));
		WriteLn(std::format("\txor {}, {}", reg.name.c_str(), reg.name.c_str()));
	}
	void OnGlobalLoad(const std::variant<RegSlot, StackSlot>& slot, const IRGenerate::GlobalValue* value) override
	{
		WriteLn("\t;; global load");
		Write("\tmov ");
		if (std::holds_alternative<RegSlot>(slot))
		{
			Write(std::get<RegSlot>(slot).name.c_str());
		}
		else
		{
			Write(std::format("[rbp-{}]", std::get<StackSlot>(slot).offset));
		}
		WriteLn(std::format(", [rel {}]", value->Name().c_str()));
	}
	void OnMove(const std::variant<RegSlot, StackSlot>& from, const std::variant<RegSlot, StackSlot>& to) override {
		WriteLn("\t;; moving");
		if (std::holds_alternative<RegSlot>(from) && std::holds_alternative<RegSlot>(to))
		{
			auto fromReg = std::get<RegSlot>(from);
			auto toReg = std::get<RegSlot>(to);
			if (fromReg.size < toReg.size)
			{
				Write("\tmovzx");
			}
			else
			{
				Write("\tmov");
			}
		}
		else
		{
			Write("\tmov");
		}
		Write(" ");
		if (std::holds_alternative<RegSlot>(to))
		{
			Write(std::get<RegSlot>(to).name.c_str());
		}
		else
		{
			Write(std::format("[rbp-{}]", std::get<StackSlot>(to).offset));
		}
		Write(", ");
		if (std::holds_alternative<RegSlot>(from))
		{
			Write(std::get<RegSlot>(from).name.c_str());
		}
		else
		{
			Write(std::format("[rbp-{}]", std::get<StackSlot>(from).offset));
		}
		WriteLn("");
	}
	void OnImmediateStore(const std::variant<RegSlot, StackSlot>& to, const IRGenerate::ImmediateValue* value) override {
		WriteLn("\t;; immediate store");
		if (std::holds_alternative<RegSlot>(to))
		{
			auto& reg = std::get<RegSlot>(to);
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

	m_handler = new GeneratorAllocatorHandler(m_output);

	RegisterDistributor* distributor = new RegisterDistributor({
		RegGroup{
			"RAX",
			{
				Reg{"al", 8},
				Reg{"ax", 16},
				Reg{"eax", 32},
				Reg{"rax", 64},
			}
		},

		RegGroup{
			"RCX",
			{
				Reg{"cl", 8},
				Reg{"cx", 16},
				Reg{"ecx", 32},
				Reg{"rcx", 64},
			}
		},

		RegGroup{
			"RDX",
			{
				Reg{"dl", 8},
				Reg{"dx", 16},
				Reg{"edx", 32},
				Reg{"rdx", 64},
			}
		},

		RegGroup{
			"R9",
			{
				Reg{"r9b", 8},
				Reg{"r9w", 16},
				Reg{"r9d", 32},
				Reg{"r9", 64},
			}
		},

		RegGroup{
			"R8",
			{
				Reg{"r8b", 8},
				Reg{"r8w", 16},
				Reg{"r8d", 32},
				Reg{"r8", 64},
			}
		},
	});

	m_allocator = new BasicSlotAllocator(distributor, m_handler);
}

void Generator::Generate()
{
    using namespace IRGenerate;
    
	WriteLn("; Symbols:\n");
	for (const Scope* scope : m_ir->Scopes())
	{
		for (auto& [_, symbol] : scope->Symbols())
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
	}

	WriteLn("\nsection .text\n");
	const Scope* globalScope = m_ir->Scopes().back();
	for (auto& [_, function] : globalScope->Functions())
	{
		WriteLn(std::format("{}:", function->Name().c_str()));
		WriteLn("\tpush rbp");
		WriteLn("\tmov rbp, rsp");

		m_allocator->ResetState();

		assert(function->Params().Size() <= 4 && "only 4 function parameters are supported for now");
		Array<String> parameterRegs = { "RCX", "RDX", "R8", "R9" };
		for (size_t i = 0; i < function->Params().Size(); ++i)
		{
			const FunctionParam* param = function->Params()[i];
			assert(!param->initialValue.has_value() && "initial values are not supported yet.");
			m_allocator->LoadValueInReg(param->destination, parameterRegs[i]);
		}

		// Stack-Alignment (16 bytes for Windows)
#if 0
		{
			// return address + old stack frame (8 + 8) = 16 % 16 = 0
			Uint32 bytesAllocated = 0;
			for (const auto& instr : function->Body()->Body())
			{
				if (const AllocaInstr* alloca = instr->As<AllocaInstr>())
				{
					auto typeSize = TypeResolver::ResolveTypeSize(alloca->Typ());
					bytesAllocated += typeSize.size / 8;
				}
				/*
				else if (const CallInstr* call = instr->As<CallInstr>())
				{
					auto typeSize = TypeResolver::ResolveTypeSize(call->RetType());
					bytesAllocated += typeSize.size / 8;
				}
				*/
			}
			Uint32 additionalBytes = bytesAllocated < 16 ? 16 - bytesAllocated : bytesAllocated % 16;
			if (additionalBytes > 0)
			{
				m_allocator->GetNewStackSlotFromSize(additionalBytes * 8);
			}
		}
#endif

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
		Write("\t;; ");
		dump_instr(instr, m_output);
		WriteLn("");
		GenerateInstr(instr);
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

			const Slot& slot = m_allocator->StoreOrUpdateValue(dest);

			// TODO: repetitive code
			{
				Write("\t;; ");
				dump_value(dest, m_output);
				if (std::holds_alternative<RegSlot>(slot.storage))
					WriteLn(std::format(" -> {}", std::get<RegSlot>(slot.storage).name.c_str()));
				else // StackSlot
					WriteLn(std::format(" -> [rbp-{}]", std::get<StackSlot>(slot.storage).offset));
			}
			// m_allocator->StoreLocal(dest);
		}
		break;
	case InstrOp::STORE:
		{
			auto store = instr->As<StoreInstr>();
			const LocalValue* dest = store->Dest()->As<LocalValue>();
			assert(dest);
			const RegSlot& slot = m_allocator->LoadValueInReg(dest);
			const RegSlot& src = m_allocator->LoadValueInReg(store->Src());
			if (src.groupName.Empty()) assert(false);
			WriteLn("\t;; store");
			WriteLn(std::format("\tmov {}, {}", slot.name.c_str(), src.name.c_str()));
			m_allocator->FreeReg(src);
		}
		break;
	case InstrOp::CALL:
		{
			auto call = instr->As<CallInstr>();
			assert(call->Args().Size() <= 4 && "only up to 4 arguments are supported for function call");
			// Array<String> parameterRegs = { "ecx", "edx", "r8d", "r9d" };
			Array<String> callerSaveRegs = { "RAX", "RCX", "RDX", "R8", "R9", "R10", "R11" };
			for (const auto& callerSaveReg : callerSaveRegs)
			{
				m_allocator->SpillRegGroup(callerSaveReg);
			}
			Array<String> parameterRegs = { "RCX", "RDX", "R8", "R9" };
			Array<RegSlot> argRegs;
			WriteLn("\t;; funcall");
			WriteLn("\t;; arguments");
			// Windows 4 x 8-byte shadow space
			for (size_t i = 0; i < call->Args().Size(); ++i)
			{
				argRegs.Push(m_allocator->LoadValueInReg(call->Args()[i], parameterRegs[i]));
			}

			RegSlot result = m_allocator->LoadValueInReg(call->Dest(), "RAX");

			// 16-byte alignment
			Uint32 currentOffset = m_allocator->GetStackOffset();
			Uint32 remainingBytes = (16 - (currentOffset % 16)) % 16;

			// 32-byte shadow space on Windows
			m_allocator->GetNewStackSlotFromSize((32 + remainingBytes) * 8);

			WriteLn("\t;; call");
			WriteLn(std::format("\tcall {}", call->CallSymbol()->Name().c_str()));

			// remove 32-byte shadow space on Windows
			m_allocator->SetExplicitOffset(m_allocator->GetStackOffset() - 32 - remainingBytes);
			WriteLn(std::format("\tadd rsp, {}", 32 + remainingBytes));

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
			WriteLn("\t;; load");
			const Slot& srcSlot = m_allocator->StoreOrUpdateValue(load->Src());
			const RegSlot& dstReg = m_allocator->LoadValueInReg(load->Dest());
			// TODO: repetitive code
			{
				Write("\t;; ");
				dump_value(load->Src(), m_output);
				if (std::holds_alternative<RegSlot>(srcSlot.storage))
					WriteLn(std::format(" -> {}", std::get<RegSlot>(srcSlot.storage).name.c_str()));
				else // StackSlot
					WriteLn(std::format(" -> [rbp-{}]", std::get<StackSlot>(srcSlot.storage).offset));
			}
			// TODO: repetitive code
			{
				Write("\t;; ");
				dump_value(load->Dest(), m_output);
				WriteLn(std::format(" -> {}", dstReg.name.c_str()));
			}
			// WriteLn(std::format("\tmov {}, {}", dstReg.name.c_str(), srcSlot.name.c_str()));
			m_handler->OnMove(srcSlot.storage, dstReg);
		}
		break;
	case InstrOp::PTR:
		{
			auto ptr = instr->As<PtrInstr>();
			WriteLn("\t;; ptr");
			const Value* src = ptr->Src();
			RegSlot dstReg = m_allocator->LoadValueInReg(ptr->Dest());
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
				const Slot& slot = m_allocator->StoreOrUpdateValue(local, StackDestination{std::nullopt});
				assert(std::holds_alternative<StackSlot>(slot.storage) && "SHOULD BE STORED ON THE STACK");
				WriteLn(std::format("\tlea {}, [rbp-{}]", dstReg.name.c_str(), std::get<StackSlot>(slot.storage).offset));
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
	for (const Scope* scope : m_ir->Scopes())
	{
		for (auto& [_, symbol] : scope->Symbols())
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

					Write(std::format("db ", std::to_string(imm->ImmValue())));
					Write(std::format("{:#04x}", imm->ImmValue()));
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

		WriteLn("");
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

