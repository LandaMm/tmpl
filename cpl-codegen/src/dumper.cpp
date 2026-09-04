#include "cpl-codegen/dumper.h"

void dump_type_name(const IRGenerate::Type* typ, FileStreamWriter* stream)
{
	using namespace IRGenerate;
	assert(typ);
	if (typ->TypClass() == TypeClass::POINTER) stream->Write(String("*"));
	stream->Write(String(typ->Name()));
}

void dump_type(const IRGenerate::Type* typ, FileStreamWriter* stream)
{
	assert(typ);
	using namespace IRGenerate;
	if (typ->TypClass() != TypeClass::ALIAS) stream->Write(String("[#"));
	switch (typ->TypClass())
	{
	case TypeClass::INTEGER:
		stream->Write(String("integer"));
		break;
	case TypeClass::FLOAT:
		stream->Write(String("float"));
		break;
	case TypeClass::FUNCTION:
		stream->Write(String("function"));
		break;
	case TypeClass::POINTER:
		stream->Write(String("pointer"));
		break;
	case TypeClass::ALIAS:
	{
		if (const AliasType* alias = dynamic_cast<const AliasType*>(typ))
		{
			dump_type_name(alias->OriginType(), stream);
		}
		else
		{
			stream->Write(String("#broken_alias"));
		}
		break;
	}
	case TypeClass::UNKNOWN:
	default:
		stream->Write(String("unknown"));
		break;
	}
	if (const SizedType* sized = dynamic_cast<const SizedType*>(typ))
	{
		stream->Write(String(" size = ") + sized->Layout().size.ToString());
		stream->Write(String(" align = ") + sized->Layout().align.ToString());
	}
	if (const PointerType* pointer = dynamic_cast<const PointerType*>(typ))
	{
		stream->Write(String(" -> { "));
		dump_type(pointer->UnderlyingType(), stream);
		stream->Write(String(" }"));
	}
	if (typ->TypClass() != TypeClass::ALIAS) stream->Write(String("]"));
}

void dump_symbol_name(const IRGenerate::Symbol* symbol, FileStreamWriter* stream)
{
	stream->Write(String("@") + String(symbol->Name()));
}

void dump_value(const IRGenerate::Value* value, FileStreamWriter* stream)
{
	using namespace IRGenerate;

	switch (value->Kind())
	{
	case ValueKind::GLOBAL:
	{
		stream->Write(String("global "));
		dump_type_name(value->Typ(), stream);
		auto global = value->As<GlobalValue>();
		stream->Write(String(" ") + global->Name());
	}
	break;
	case ValueKind::TEMPORAL:
	{
		const TemporalValue* temp = dynamic_cast<const TemporalValue*>(value);
		assert(temp);
		stream->Write(String("%") + String(std::to_string(temp->Id())));
		break;
	}
	case ValueKind::LOCAL:
	{
		const LocalValue* local = dynamic_cast<const LocalValue*>(value);
		assert(local);
		stream->Write(String("@") + String(local->Name()));
		break;
	}
	case ValueKind::IMMEDIATE:
	{
		const ImmediateValue* immediate = dynamic_cast<const ImmediateValue*>(value);
		assert(immediate);
		auto integerDescription = TypeResolver::Integer(immediate->Typ());
		assert(integerDescription);
		stream->Write(
			String("i") +
			integerDescription->Layout().size.ToString() +
			String(" #")
		);
		Int64 value = immediate->ImmValue();
		stream->Write(String(std::to_string(value)));
		break;
	}
	case ValueKind::UNKNOWN:
	default:
		stream->Write(String("#unknown_value "));
		break;
	}

}

void dump_symbol(const IRGenerate::Symbol* symbol, FileStreamWriter* stream)
{
	using namespace IRGenerate;

	dump_symbol_name(symbol, stream);

	/*
	stream->Write(String(" ("));
	switch (symbol.SymType())
	{
	case SymbolType::VARIABLE:
		stream->Write(String("variable"));
		break;
	case SymbolType::FUNCTION:
		stream->Write(String("function"));
		break;
	case SymbolType::UNKNOWN:
	default:
		stream->Write(String("unknown"));
		break;
	}
	stream->Write(String(")"));
	*/

	stream->Write(String(": "));
	if (auto function = dynamic_cast<const Symbols::Function*>(symbol))
	{
		const FunctionType* funcType = function->FunctionDescription();
		stream->Write(String("("));
		for (size_t i = 0; i < funcType->Params().Size(); ++i)
		{
			const auto param = funcType->Params()[i];
			if (i > 0)
			{
				stream->Write(String(", "));
			}
			stream->Write(String(param.name) + String(": "));
			dump_type_name(param.typ, stream);
		}
		stream->Write(String(") -> "));
		dump_type_name(funcType->RetType(), stream);
	}
	else
	{
		dump_type_name(symbol->Typ(), stream);
	}

	if (symbol->Origin() == SymbolOrigin::FOREIGN)
	{
		stream->Write(String(" extrn"));
	}
}

void dump_instr(const IRGenerate::Instr* instr, FileStreamWriter* stream)
{
	using namespace IRGenerate;
	if (instr->Valued())
	{
		dump_value(instr->Dest(), stream);
		stream->Write(String(" = "));
	}
	switch (instr->Type())
	{
	case InstrOp::CALL:
	{
		auto callInstr = dynamic_cast<const CallInstr*>(instr);
		assert(callInstr);
		dump_type_name(callInstr->RetType(), stream);
		stream->Write(String(" call "));
		dump_symbol_name(callInstr->CallSymbol(), stream);
		stream->Write(String("("));
		for (size_t i = 0; i < callInstr->Args().Size(); ++i)
		{
			if (i > 0)
			{
				stream->Write(String(", "));
			}
			dump_value(callInstr->Args()[i], stream);
		}
		stream->Write(String(")"));
		break;
	}
	case InstrOp::ALLOCA:
	{
		auto allocaInstr = dynamic_cast<const AllocaInstr*>(instr);
		assert(allocaInstr);
		stream->Write(String("alloca "));
		dump_type_name(allocaInstr->Typ(), stream);
		break;
	}
	case InstrOp::STORE:
	{
		auto storeInstr = dynamic_cast<const StoreInstr*>(instr);
		assert(storeInstr);
		dump_type_name(storeInstr->Typ(), stream);
		stream->Write(String(" store "));
		dump_value(storeInstr->Src(), stream);
		break;
	}
	case InstrOp::LOAD:
	{
		auto loadInstr = dynamic_cast<const LoadInstr*>(instr);
		assert(loadInstr);
		dump_type_name(loadInstr->Typ(), stream);
		stream->Write(String(" load "));
		dump_value(loadInstr->Src(), stream);
		break;
	}
	case InstrOp::PTR:
	{
		auto ptrInstr = dynamic_cast<const PtrInstr*>(instr);
		assert(ptrInstr);
		dump_type_name(ptrInstr->Typ(), stream);
		stream->Write(String(" ptr "));
		dump_value(ptrInstr->Src(), stream);
		break;
	}
	case InstrOp::ADD:
	{
		auto addInstr = dynamic_cast<const AddInstr*>(instr);
		assert(addInstr);
		dump_type_name(addInstr->Dest()->Typ(), stream);
		stream->Write(String(" add "));
		dump_value(addInstr->Left(), stream);
		stream->Write(String(", "));
		dump_value(addInstr->Right(), stream);
		break;
	}
	case InstrOp::SUB:
	{
		auto subInstr = dynamic_cast<const SubInstr*>(instr);
		assert(subInstr);
		dump_type_name(subInstr->Dest()->Typ(), stream);
		stream->Write(String(" sub "));
		dump_value(subInstr->Left(), stream);
		stream->Write(String(", "));
		dump_value(subInstr->Right(), stream);
		break;
	}
	case InstrOp::MUL:
	{
		auto mulInstr = dynamic_cast<const MulInstr*>(instr);
		assert(mulInstr);
		dump_type_name(mulInstr->Dest()->Typ(), stream);
		stream->Write(String(" mul "));
		dump_value(mulInstr->Left(), stream);
		stream->Write(String(", "));
		dump_value(mulInstr->Right(), stream);
		break;
	}
	case InstrOp::DIV:
	{
		auto divInstr = dynamic_cast<const DivInstr*>(instr);
		assert(divInstr);
		dump_type_name(divInstr->Dest()->Typ(), stream);
		stream->Write(String(" div "));
		dump_value(divInstr->Left(), stream);
		stream->Write(String(", "));
		dump_value(divInstr->Right(), stream);
		break;
	}
	case InstrOp::NEG:
	{
		auto negInstr = dynamic_cast<const NegInstr*>(instr);
		assert(negInstr);
		dump_type_name(negInstr->Dest()->Typ(), stream);
		stream->Write(String(" neg "));
		dump_value(negInstr->Target(), stream);
		break;
	}
	case InstrOp::RET:
	{
		auto retInstr = dynamic_cast<const RetInstr*>(instr);
		assert(retInstr);
		stream->Write(String("ret "));
		dump_type_name(retInstr->RetValue()->Typ(), stream);
		break;
	}
	case InstrOp::NONE:
	default:
		stream->Write(String("#none_instr"));
		break;
	}
}

void dump_block(const IRGenerate::BasicBlock* block, FileStreamWriter* stream)
{
	assert(block);
	for (auto& instr : block->Body())
	{
		stream->Write(String("  "));
		dump_instr(instr, stream);
		stream->Write(String("\n"));
	}
}

void dump_ir(const IRGenerate::IR* ir, FileStreamWriter* stream)
{
	using namespace IRGenerate;

	stream->Write(String("; Types:\n\n"));

	for (const Scope* scope : ir->Scopes())
	{
		for (auto& [key, value] : scope->Types())
		{
			stream->Write(String(key) + String(": "));
			dump_type(value, stream);
			stream->Write(String("\n"));
		}
	}

	stream->Write(String("\n"));

	stream->Write(String("; Symbols:\n\n"));

	for (const Scope* scope : ir->Scopes())
	{
		for (auto& [key, value] : scope->Symbols())
		{
			dump_symbol(value, stream);
			stream->Write(String("\n"));
		}
	}

	stream->Write(String("\n"));

	stream->Write(String("; Functions:\n\n"));

	for (const Scope* scope : ir->Scopes())
	{
		for (auto& [key, value] : scope->Functions())
		{
			stream->Write(String(key) + String(" :: "));
			stream->Write(String("("));

			auto funcDesc = value->FunctionDescription();

			for (size_t i = 0; i < funcDesc->Params().Size(); ++i)
			{
				const auto param = funcDesc->Params()[i];

				if (i > 0)
					stream->Write(String(", "));

				stream->Write(String(param.name) + String(": "));
				dump_type_name(param.typ, stream);
			}

			stream->Write(String(") -> "));
			dump_type_name(funcDesc->RetType(), stream);

			if (value->Body())
			{
				stream->Write(String(":\n"));
				dump_block(value->Body(), stream);
			}

			stream->Write(String("\n"));
		}
	}

	stream->Write(String("\n"));
}
