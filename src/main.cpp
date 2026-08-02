#include <print>
#include <cassert>
#include <memory>

#include <cpl-parser/lexer.h>
#include <cpl-parser/parser.h>
#include <cpl-parser/node/program.hpp>
#include <cpl-parser/cli.h>
#include <cpl-parser/error.h>
#include <cpl-basics/allocator/arena.hpp>

#include <cpl-ir/compiler.h>

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
    delete lexer;

    auto root = reinterpret_cast<AST::Nodes::ProgramNode*>(parser->GetRoot());
    (void)root;

    auto compiler = new Compiler();

    delete parser;

    // std::string procName = cliRunner.GetProcedureName();
    // std::vector<std::string> args = cliRunner.GetProcedureArgs();
	return 0;
}
