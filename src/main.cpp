#include <print>
#include <cassert>
#include <memory>

#include <cpl-parser/lexer.h>
#include <cpl-parser/parser.h>
#include <cpl-parser/node/program.hpp>
#include <cpl-parser/cli.h>
#include <cpl-parser/error.h>
#include <cpl-basics/allocator/arena.hpp>

#include <cpl-ir/instr/call.hpp>
#include <cpl-ir/instr/alloca.hpp>
#include <cpl-ir/instr/store.hpp>
#include <cpl-ir/instr/load.hpp>
#include <cpl-ir/ir.h>

void dump_type_name(const IRGenerate::Type* typ)
{
	using namespace IRGenerate;
	assert(typ);
	if (typ->TypClass() == TypeClass::POINTER) std::cout << '*';
	std::cout << typ->Name();
}

void dump_type(const IRGenerate::Type* typ)
{
	assert(typ);
	using namespace IRGenerate;
	if (typ->TypClass() != TypeClass::ALIAS) std::cout << "[#";
	switch (typ->TypClass())
	{
	case TypeClass::INTEGER:
		std::cout << "integer";
		break;
	case TypeClass::FLOAT:
		std::cout << "float";
		break;
	case TypeClass::FUNCTION:
		std::cout << "function";
		break;
	case TypeClass::POINTER:
		std::cout << "pointer";
		break;
	case TypeClass::ALIAS:
	{
		if (const AliasType* alias = dynamic_cast<const AliasType*>(typ))
		{
			dump_type_name(alias->OriginType());
		}
		else
		{
			std::cout << "#broken_alias";
		}
		break;
	}
	case TypeClass::UNKNOWN:
	default:
		std::cout << "unknown";
		break;
	}
	if (const SizedType* sized = dynamic_cast<const SizedType*>(typ))
	{
		std::cout << " size = " << sized->Layout().size;
		std::cout << " align = " << sized->Layout().align;
	}
	if (const PointerType* pointer = dynamic_cast<const PointerType*>(typ))
	{
		std::cout << " -> { ";
		dump_type(pointer->UnderlyingType());
		std::cout << " }";
	}
	if (typ->TypClass() != TypeClass::ALIAS) std::cout << ']';
}

void dump_symbol_name(const IRGenerate::Symbol* symbol)
{
	std::cout << '@' << symbol->Name();
}

void dump_value(const IRGenerate::Value* value)
{
	using namespace IRGenerate;

	switch (value->Kind())
	{
	case ValueKind::GLOBAL:
		std::cout << "global ";
		dump_type_name(value->Typ());
		break;
	case ValueKind::TEMPORAL:
	{
		const TemporalValue* temp = dynamic_cast<const TemporalValue*>(value);
		assert(temp);
		std::cout << '%' << temp->Id();
		break;
	}
	case ValueKind::LOCAL:
	{
		const LocalValue* local = dynamic_cast<const LocalValue*>(value);
		assert(local);
		std::cout << '@' << local->Name();
		break;
	}
	case ValueKind::IMMEDIATE:
	{
		const ImmediateValue* immediate = dynamic_cast<const ImmediateValue*>(value);
		assert(immediate);
		auto integerDescription = TypeResolver::Integer(immediate->Typ());
		assert(integerDescription);
		std::cout << '#';
		auto value = immediate->ImmValue();
		switch (integerDescription->Layout().size)
		{
		case 8:
			std::cout << *static_cast<const Uint8*>(value);
			break;
		case 16:
			std::cout << *static_cast<const Uint16*>(value);
			break;
		case 32:
			std::cout << *static_cast<const Uint32*>(value);
			break;
		case 64:
			std::cout << *static_cast<const Uint64*>(value);
			break;
		default:
			assert(false && "UNSUPPORTED integer size");
		}
		break;
	}
	case ValueKind::UNKNOWN:
	default:
		std::cout << "#unknown_value ";
		break;
	}
}

void dump_symbol(const IRGenerate::Symbol* symbol)
{
	using namespace IRGenerate;

	dump_symbol_name(symbol);
	/*
	std::cout << " (";
	switch (symbol.SymType())
	{
	case SymbolType::VARIABLE:
		std::cout << "variable";
		break;
	case SymbolType::FUNCTION:
		std::cout << "function";
		break;
	case SymbolType::UNKNOWN:
	default:
		std::cout << "unknown";
		break;
	}
	std::cout << ")";
	*/
	std::cout << ": ";
	if (auto function = dynamic_cast<const Symbols::Function*>(symbol))
	{
		const FunctionType* funcType = function->FunctionDescription();
		std::cout << "(";
		for (size_t i = 0; i < funcType->Params().Size(); ++i)
		{
			const auto param = funcType->Params()[i];
			if (i > 0)
			{
				std::cout << ", ";
			}
			std::cout << param.name << ": ";
			dump_type_name(param.typ);
		}
		std::cout << ") -> ";
		dump_type_name(funcType->RetType());
	}
	else
	{
		dump_type_name(symbol->Typ());
	}

	if (symbol->Origin() == SymbolOrigin::FOREIGN)
	{
		std::cout << " extrn";
	}
}

void dump_instr(const IRGenerate::Instr* instr)
{
	using namespace IRGenerate;
	if (instr->Valued())
	{
		dump_value(instr->Dest());
		std::cout << " = ";
	}
	switch (instr->Type())
	{
	case InstrOp::CALL:
	{
		auto callInstr = dynamic_cast<const CallInstr*>(instr);
		assert(callInstr);
		dump_type_name(callInstr->RetType());
		std::cout << " call ";
		dump_symbol_name(callInstr->CallSymbol());
		std::cout << '(';
		for (size_t i = 0; i < callInstr->Args().Size(); ++i)
		{
			if (i > 0)
			{
				std::cout << ", ";
			}
			dump_value(callInstr->Args()[i]);
		}
		std::cout << ')';
		break;
	}
	case InstrOp::ALLOCA:
	{
		auto allocaInstr = dynamic_cast<const AllocaInstr*>(instr);
		assert(allocaInstr);
		std::cout << "alloca ";
		dump_type_name(allocaInstr->Typ());
		break;
	}
	case InstrOp::STORE:
	{
		auto storeInstr = dynamic_cast<const StoreInstr*>(instr);
		assert(storeInstr);
		dump_type_name(storeInstr->Typ());
		std::cout << " store ";
		dump_value(storeInstr->Src());
		break;
	}
	case InstrOp::LOAD:
	{
		auto loadInstr = dynamic_cast<const LoadInstr*>(instr);
		assert(loadInstr);
		dump_type_name(loadInstr->Typ());
		std::cout << " load ";
		dump_value(loadInstr->Src());
		break;
	}
	case InstrOp::NONE:
	default:
		std::cout << "#none_instr";
		break;
	}
}

void dump_block(const IRGenerate::BasicBlock* block)
{
	assert(block);
	for (auto& instr : block->Body())
	{
		std::cout << "  ";
		dump_instr(instr);
		std::cout << '\n';
	}
}

void dump_ir(const IRGenerate::IR* ir)
{
    using namespace IRGenerate;
    
    std::cout << "; Types:\n\n";
	for (const Scope* scope : ir->Scopes())
	{
		for (auto& [key, value] : scope->Types())
		{
			std::cout << key << ": ";
			dump_type(value);
			std::cout << '\n';
		}
	}
	std::cout << '\n';

	std::cout << "; Symbols:\n\n";
	for (auto& [key, value] : ir->Symbols())
	{
		dump_symbol(value);
		std::cout << '\n';
	}
	std::cout << '\n';

	std::cout << "; Functions:\n\n";
	for (const Scope* scope : ir->Scopes())
	{
		for (auto& [key, value] : scope->Functions())
		{
			std::cout << key << " :: ";
			std::cout << "(";
			auto funcDesc = value->FunctionDescription();
			for (size_t i = 0; i < funcDesc->Params().Size(); ++i)
			{
				const auto param = funcDesc->Params()[i];
				if (i > 0) std::cout << ", ";
				std::cout << param.name << ": ";
				dump_type_name(param.typ);
			}
			std::cout << ") -> ";
			dump_type_name(funcDesc->RetType());
			if (value->Body()) {
				std::cout << ":\n";
				dump_block(value->Body());
			}
			std::cout << '\n';
		}
	}
	std::cout << '\n';

}

int main(int argc, char **argv)
{
	using namespace AST;
	using namespace Runtime;

    CliRunner cliRunner(argc, argv);

    String filename = cliRunner.GetScriptFilename();

	FileReader reader(filename.c_str());

    Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();

    if (!reader.Ok())
    {
        errManager.FailedOpeningFile(filename);
        return 1;
    }

	Lexer* lexer = new Lexer(reader, filename);
	lexer->Tokenize();

	Parser* parser = new Parser(lexer);
	parser->Parse();

    auto root = reinterpret_cast<AST::Nodes::ProgramNode*>(parser->GetRoot());
    (void)root;

    auto compiler = new IRGenerate::IR(root);

    compiler->GenerateIR();

	dump_ir(compiler);

    delete parser;

    // std::string procName = cliRunner.GetProcedureName();
    // std::vector<std::string> args = cliRunner.GetProcedureArgs();
	return 0;
}
