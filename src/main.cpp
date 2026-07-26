
#include <cassert>
#include <memory>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/node/program.hpp"
#include "../include/cli.h"
#include "../include/error.h"

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

	std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(reader, filename);
	lexer->Tokenize();

	std::shared_ptr<Parser> parser = std::make_shared<Parser>(lexer);
	parser->Parse();

    auto root = reinterpret_pointer_cast<AST::Nodes::ProgramNode>(parser->GetRoot());
    (void)root;

    // std::string procName = cliRunner.GetProcedureName();
    // std::vector<std::string> args = cliRunner.GetProcedureArgs();
	return 0;
}
