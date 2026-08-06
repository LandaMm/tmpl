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

void dump_type(const IRGenerate::Type* typ)
{
	using namespace IRGenerate;
	std::cout << "[#";
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
	std::cout << ']';
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
	dump_type(symbol.ValueType());
}

void dump_instr(const IRGenerate::Instr* instr)
{
	using namespace IRGenerate;
	switch (instr->Type())
	{
	case InstrOp::CALL:
	{
		auto callInstr = dynamic_cast<const CallInstr*>(instr);
		std::cout << "call " << callInstr->CallSymbol().Name();
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
			dump_type(param.typ);
		}
		std::cout << ") -> ";
		dump_type(value->RetType());
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
