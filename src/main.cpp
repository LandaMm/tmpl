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
#include <cpl-ir/ir.h>

void dump_type_name(const IRGenerate::Type* typ)
{
	assert(typ);
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
			std::cout << alias->OriginType()->Name();
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

void dump_value(const IRGenerate::Value* value)
{
	using namespace IRGenerate;

	switch (value->Kind())
	{
	case ValueKind::GLOBAL:
		std::cout << "global ";
		break;
	case ValueKind::UNKNOWN:
	default:
		std::cout << "#unknown_value ";
		break;
	}

	dump_type_name(value->Typ());
}

void dump_symbol(const IRGenerate::Symbol& symbol)
{
	using namespace IRGenerate;

	if (symbol.Origin() == SymbolOrigin::FOREIGN)
	{
		std::cout << "extrn ";
	}
	std::cout << symbol.Name();
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
	dump_type_name(symbol.ValueType());
}

void dump_instr(const IRGenerate::Instr* instr)
{
	using namespace IRGenerate;
	switch (instr->Type())
	{
	case InstrOp::CALL:
	{
		auto callInstr = dynamic_cast<const CallInstr*>(instr);
		dump_type_name(callInstr->RetType());
		std::cout << " call " << callInstr->CallSymbol().Name() << '(';
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
	case InstrOp::LOAD:
		assert(false && "LOAD not implemented yet");
		break;
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
    for (auto& [key, value] : ir->Types())
    {
        std::cout << key << ": ";
		dump_type(value);
		std::cout << '\n';
    }
	std::cout << '\n';

	std::cout << "; Symbols:\n\n";
	for (auto& [key, value] : ir->Symbols())
	{
		dump_symbol(*value);
		std::cout << '\n';
	}
	std::cout << '\n';

	std::cout << "; Functions:\n\n";
	for (auto& [key, value] : ir->Functions())
	{
		std::cout << key << " :: ";
		std::cout << "(";
		for (size_t i = 0; i < value->Params().Size(); ++i)
		{
			auto& param = value->Params().At(i);
			if (i > 0) std::cout << ", ";
			std::cout << param.name << ": ";
			dump_type_name(param.typ);
		}
		std::cout << ") -> ";
		dump_type_name(value->RetType());
		if (value->Body()) {
			std::cout << ":\n";
			dump_block(value->Body());
		}
		std::cout << '\n';
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
