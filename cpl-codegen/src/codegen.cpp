#include <format>

#include "cpl-codegen/codegen.h"
#include "cpl-codegen/dumper.h"

namespace Codegen
{

Generator::Generator(FileStreamWriter* output, const IRGenerate::IR* ir)
		: m_ir(ir), m_output(output)
{
	using namespace IRGenerate;
	AllocatorHandler handler;
	handler.OnNewStackSlot([this](const StackSlot& slot) {
		WriteLn("\t;; new stack slot");
		WriteLn(std::format("\tsub rsp, {}", slot.size));
	});
	handler.OnSpill([this](const Reg& from, const StackSlot& to) {
		WriteLn("\t;; register spill");
		WriteLn(std::format("\tmov [rbp-{}], {}", to.offset, from.name.c_str()));
		WriteLn(std::format("\txor {}, {}", from.name.c_str(), from.name.c_str()));
	});
	handler.OnImmediateStore([this](const std::variant<Reg, StackSlot>& to, const ImmediateValue* value) {
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
	});

	m_allocator = new BasicSlotAllocator({
		Reg{"eax", 32},
		Reg{"ax", 16},
		Reg{"al", 8},

		Reg{"ecx", 32},
		Reg{"cx", 16},
		Reg{"cl", 8},
	}, handler);
}

void Generator::Generate()
{
    using namespace IRGenerate;

	WriteLn("format ELF64");
    
	WriteLn("; Symbols:\n");
	for (auto& [_, symbol] : m_ir->Symbols())
	{
		if (symbol->Origin() != SymbolOrigin::FOREIGN)
		{
			WriteLn(std::format("public {}", symbol->Name().c_str()));
		}
		else
		{
			WriteLn(std::format("extrn {}", symbol->Name().c_str()));
		}
	}

	WriteLn("\nsection '.code' executable\n");
	const Scope* globalScope = m_ir->Scopes().back();
	for (auto& [_, function] : globalScope->Functions())
	{
		WriteLn(std::format("{}:", function->Name().c_str()));
		WriteLn("\tpush rbp");
		WriteLn("\tmov rbp, rsp");

		GenerateBasicBlock(function->Body());

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
			// Handled by AllocatorHandler
			// WriteLn(std::format("\tsub rsp, {}", slot.size));
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
				WriteLn("");
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

void Generator::Write(const String& what)
{
	m_output->Write(what);
}

void Generator::WriteLn(const String& what)
{
	m_output->Write(what + "\n");
}

} // namespace Codegen

