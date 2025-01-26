
#include <iostream>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interpreter.h"
#include "../include/interpreter/environment.h"

int main(int argc, char **argv)
{
	using namespace AST;
	using namespace Runtime;

	if (argc < 2)
	{
		Prelude::ErrorManager &errManager = Prelude::ErrorManager::getInstance();
		errManager.NoInputFile();
		return 1;
	}

	std::ifstream input(argv[1]);

	std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(input);

	lexer->Tokenize();

	std::shared_ptr<Parser> parser = std::make_shared<Parser>(lexer);
	parser->Parse();

	std::shared_ptr<Environment> env = std::make_shared<Environment>();
	Interpreter intrpt(parser, env);

	std::shared_ptr<ProgramNode> program = std::dynamic_pointer_cast<ProgramNode>(parser->GetRoot());
	std::vector<std::shared_ptr<Value>> values;

	for (size_t i = 0; i < program->Size(); i++)
	{
		auto value = intrpt.Evaluate((*program)[i]);
		std::cout << "value: " << value << std::endl;
		values.push_back(value);
	}

	return 0;
}
