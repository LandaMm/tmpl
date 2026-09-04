#include <format>

#include "cpl-codegen/codegen.h"
#include "cpl-codegen/dumper.h"

namespace Codegen
{

class GeneratorAllocatorHandler : public AllocatorHandler
{
public:
	GeneratorAllocatorHandler(FileStreamWriter* output) : m_output(output) { }

	using Slot = std::variant<RegSlot, StackSlot>;
private:
	void Write(const String& what)
	{
		m_output->Write(what);
	}

	void WriteLn(const String& what)
	{
		m_output->Write(what + "\n");
	}

	MemSize GetSlotSize(const Slot& slot)
	{
		if (std::holds_alternative<RegSlot>(slot))
		{
			return std::get<RegSlot>(slot).size;
		}
		else if (std::holds_alternative<StackSlot>(slot))
		{
			return std::get<StackSlot>(slot).size;
		}

		assert(false && "UNHANDLED SLOT KIND");
		return {};
	}

	// TODO: think if global values should have their own slot kind
	// for example like StackSlot but DataSlot(name: str)
	String GetValueSizeName(const IRGenerate::Type* typ)
	{
		auto layout = IRGenerate::TypeResolver::ResolveTypeSize(typ);
		switch (layout.size.Bits())
		{
		case 8:
			return "byte";
		case 16:
			return "word";
		case 32:
			return "dword";
		case 64:
			return "qword";
		default:
			// TODO: better error maybe
			assert(false && "unsupported type size");
		}

		return {};
	}

	String GetStackSlotSizeName(const StackSlot& stackSlot)
	{
		switch (stackSlot.size.Bits())
		{
		case 8:
			return "byte";
		case 16:
			return "word";
		case 32:
			return "dword";
		case 64:
			return "qword";
		default:
			// TODO: better error maybe
			assert(false && "invalid stack slot size");
		}

		return {};
	}

	String GetSlotOperand(const Slot& slot)
	{
		if (std::holds_alternative<RegSlot>(slot))
		{
			return std::get<RegSlot>(slot).name;
		}
		else if (std::holds_alternative<StackSlot>(slot))
		{
			const auto& stackSlot = std::get<StackSlot>(slot);
			return std::format("{} [rbp-{}]", GetStackSlotSizeName(stackSlot), stackSlot.offset.Bytes());
		}

		assert(false && "UNHANDLED SLOT KIND");
		return {};
	}

	void MoveSlots(const Slot& from, const Slot& to)
	{
		auto lSize = GetSlotSize(from);
		auto rSize = GetSlotSize(to);
		Write("\t");
		Write(rSize > lSize ? "movzx" : "mov");
		Write(" ");
		Write(GetSlotOperand(to));
		Write(", ");
		Write(GetSlotOperand(from));
		WriteLn("");
	}
public:
	void OnNewStackSlot(const StackSlot& slot) override {
		WriteLn("\t;; new stack slot");
		WriteLn(std::format("\tsub rsp, {}", slot.size.Bytes()));
	}
	void OnSpill(const RegSlot& from, const StackSlot& to) override {
		WriteLn("\t;; register spill");
		MoveSlots(from, to);
	}
	void OnFreeReg(const RegSlot& reg) override {
		// TODO: think if we need to do anything here
		WriteLn(std::format("\t;; free {} reg", reg.name.c_str()));
		WriteLn(std::format("\txor {}, {}", reg.name.c_str(), reg.name.c_str()));
	}
	void OnGlobalLoad(const std::variant<RegSlot, StackSlot>& slot, const IRGenerate::GlobalValue* value) override
	{
		WriteLn("\t;; global load");
		Write("\tmov ");
		Write(GetSlotOperand(slot));
		WriteLn(std::format(", {} [rel {}]", GetValueSizeName(value->Typ()), value->Name().c_str()));
	}
	void OnMove(const std::variant<RegSlot, StackSlot>& from, const std::variant<RegSlot, StackSlot>& to) override {
		WriteLn("\t;; moving");
		MoveSlots(from, to);
	}
	void OnImmediateStore(const std::variant<RegSlot, StackSlot>& to, const IRGenerate::ImmediateValue* value) override {
		WriteLn("\t;; immediate store");
		WriteLn(std::format("\tmov {}, {}", GetSlotOperand(to), value->ImmValue()));
	}
	void OnStackRelease(MemSize offset) override
	{
		WriteLn("\t;; release stack by offset");
		WriteLn(std::format("\tadd rsp, {}", offset.Bytes()));
	}
private:
	FileStreamWriter* m_output;
};

Generator::Generator(FileStreamWriter* output, const IRGenerate::IR* ir)
		: m_ir(ir), m_output(output)
{
	using namespace IRGenerate;

	m_handler = new GeneratorAllocatorHandler(m_output);

	RegisterDistributor* distributor = m_arena.Alloc<RegisterDistributor>(std::initializer_list<RegGroup>{
		RegGroup{
			"RAX",
			{
				Reg{"al", MemSize::FromBits(8)},
				Reg{"ax", MemSize::FromBits(16)},
				Reg{"eax", MemSize::FromBits(32)},
				Reg{"rax", MemSize::FromBits(64)},
			}
		},

		RegGroup{
			"RCX",
			{
				Reg{"cl", MemSize::FromBits(8)},
				Reg{"cx", MemSize::FromBits(16)},
				Reg{"ecx", MemSize::FromBits(32)},
				Reg{"rcx", MemSize::FromBits(64)},
			}
		},

		RegGroup{
			"RDX",
			{
				Reg{"dl", MemSize::FromBits(8)},
				Reg{"dx", MemSize::FromBits(16)},
				Reg{"edx", MemSize::FromBits(32)},
				Reg{"rdx", MemSize::FromBits(64)},
			}
		},

		RegGroup{
			"R9",
			{
				Reg{"r9b", MemSize::FromBits(8)},
				Reg{"r9w", MemSize::FromBits(16)},
				Reg{"r9d", MemSize::FromBits(32)},
				Reg{"r9", MemSize::FromBits(64)},
			}
		},

		RegGroup{
			"R8",
			{
				Reg{"r8b", MemSize::FromBits(8)},
				Reg{"r8w", MemSize::FromBits(16)},
				Reg{"r8d", MemSize::FromBits(32)},
				Reg{"r8", MemSize::FromBits(64)},
			}
		},
	});

	StackDistributor* stackDistro = m_arena.Alloc<StackDistributor>(m_handler);

	m_allocator = new BasicSlotAllocator(distributor, stackDistro, m_handler);
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

		GenerateBasicBlock(function->Body());

		// TODO: repetitive code
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
					WriteLn(std::format(" -> [rbp-{}]", std::get<StackSlot>(slot.storage).offset.Bytes()));
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
			m_allocator->FreeValueWithReg(src);
		}
		break;
	case InstrOp::CALL:
		{
			auto call = instr->As<CallInstr>();
			assert(call->Args().Size() <= 4 && "only up to 4 arguments are supported for function call");
			// Array<String> parameterRegs = { "ecx", "edx", "r8d", "r9d" };
			constexpr MemSize PARAMETER_MIN_SIZE = MemSize::FromBits(32);
			Array<String> parameterRegs = { "RCX", "RDX", "R8", "R9" };
			Array<RegSlot> argRegs;
			WriteLn("\t;; funcall");
			WriteLn("\t;; arguments");
			// Windows 4 x 8-byte shadow space
			for (size_t i = 0; i < parameterRegs.Size(); ++i)
			{
				m_allocator->SpillRegGroup(parameterRegs[i]);
				if (i < call->Args().Size())
					argRegs.Push(m_allocator->LoadValueInReg(call->Args()[i], RegTarget(parameterRegs[i], PARAMETER_MIN_SIZE)));
			}

			// Safely free the RAX register for return value
			// m_allocator->SpillRegGroup("RAX");
			m_allocator->LoadValueInReg(call->Dest(), RegTarget("RAX", MemSize::FromBits(32)));

			// Caller Saved Registers
			{
				Array<String> callerSaveRegs = { "R10", "R11" };
				for (const auto& callerSaveReg : callerSaveRegs)
				{
					m_allocator->SpillRegGroup(callerSaveReg);
				}
			}

			if (call->CallSymbol()->Name() == "exit")
			{
				WriteLn("\t;; gotcha!");
			}
			const auto alignment = m_allocator->AlignStack(16);
			// 32-byte shadow space on Windows
			const auto& shadowSpace = m_allocator->AllocateTempStackSlot(MemSize::FromBits(32) * 8);

			WriteLn("\t;; call");
			WriteLn("\txor rax, rax");
			WriteLn(std::format("\tcall {}", call->CallSymbol()->Name().c_str()));

			// remove 32-byte shadow space on Windows
			m_allocator->ReleaseTempStackSlot(shadowSpace);
			if (alignment) m_allocator->ReleaseTempStackSlot(alignment.value());

			// free argument registers
			for (auto &reg : argRegs)
			{
				m_allocator->FreeValueWithReg(reg);
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
					WriteLn(std::format(" -> [rbp-{}]", std::get<StackSlot>(srcSlot.storage).offset.Bytes()));
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
				WriteLn(std::format("\tlea {}, [rbp-{}]", dstReg.name.c_str(), std::get<StackSlot>(slot.storage).offset.Bytes()));
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
	case InstrOp::ADD:
	{
		auto add = instr->As<AddInstr>();
		auto dest = m_allocator->LoadValueInReg(add->Dest());
		auto left = m_allocator->LoadValueInReg(add->Left());
		auto right = m_allocator->LoadValueInReg(add->Right());
		m_handler->OnMove(left, dest);
		WriteLn(std::format("\tadd {}, {}", dest.name.c_str(), right.name.c_str()));
		m_allocator->FreeValueWithReg(left);
		m_allocator->FreeValueWithReg(right);
	}
	break;
	case InstrOp::SUB:
	{
		auto sub = instr->As<SubInstr>();
		auto dest = m_allocator->LoadValueInReg(sub->Dest());
		auto left = m_allocator->LoadValueInReg(sub->Left());
		auto right = m_allocator->LoadValueInReg(sub->Right());
		m_handler->OnMove(left, dest);
		WriteLn(std::format("\tsub {}, {}", dest.name.c_str(), right.name.c_str()));
		m_allocator->FreeValueWithReg(left);
		m_allocator->FreeValueWithReg(right);
	}
	break;
	case InstrOp::MUL:
	{
		auto mul = instr->As<MulInstr>();
		auto dest = m_allocator->LoadValueInReg(mul->Dest());
		auto left = m_allocator->LoadValueInReg(mul->Left());
		auto right = m_allocator->LoadValueInReg(mul->Right());
		m_handler->OnMove(left, dest);
		WriteLn(std::format("\timul {}, {}", dest.name.c_str(), right.name.c_str()));
		m_allocator->FreeValueWithReg(left);
		m_allocator->FreeValueWithReg(right);
	}
	break;
	case InstrOp::DIV:
	{
		auto div = instr->As<DivInstr>();
		auto left = m_allocator->LoadValueInReg(div->Left());
		auto right = m_allocator->LoadValueInReg(div->Right());
		auto dest = m_allocator->LoadValueInReg(div->Dest(), RegTarget("RAX"));
		m_handler->OnMove(left, dest);
		WriteLn("\tcdq");
		WriteLn(std::format("\tidiv {}", right.name.c_str()));
		m_allocator->FreeValueWithReg(left);
		m_allocator->FreeValueWithReg(right);
	}
	break;
	case InstrOp::NEG:
	{
		auto neg = instr->As<NegInstr>();
		auto target = m_allocator->LoadValueInReg(neg->Target());
		auto dest = m_allocator->LoadValueInReg(neg->Dest());
		m_handler->OnMove(target, dest);
		WriteLn(std::format("\tneg {}", dest.name));
		m_allocator->FreeValueWithReg(target);
	}
	break;
	case InstrOp::RET:
	{
		auto ret = instr->As<RetInstr>();
		m_allocator->LoadValueInReg(ret->RetValue(), RegTarget("RAX", MemSize::FromBits(32)));
		// TODO: repetitive code
		WriteLn("\tmov rsp, rbp");
		WriteLn("\tpop rbp");
		WriteLn("\tret");
	}
	break;
	case InstrOp::NONE:
	default:
		// TODO: better error
		assert(false && "unsupported instruction type encountered");
		WriteLn(std::format("\t;; TODO: instr op {}", static_cast<int>(instr->Type())));
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

