
#include <cassert>
#include <memory>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/cli.h"
#include "../include/error.h"

int main(int argc, char **argv)
{
	using namespace AST;
	using namespace Runtime;

    CliRunner cliRunner(argc, argv);

    std::string filename = cliRunner.GetScriptFilename();

	std::ifstream input(filename);

    Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();

    if (!input.good())
    {
        errManager.FailedOpeningFile(filename);
        return 1;
    }

	std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(input, filename);
	lexer->Tokenize();

	std::shared_ptr<Parser> parser = std::make_shared<Parser>(lexer);
	parser->Parse();

    // std::string procName = cliRunner.GetProcedureName();
    // std::vector<std::string> args = cliRunner.GetProcedureArgs();
	return 0;
}
