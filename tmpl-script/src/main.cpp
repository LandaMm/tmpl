
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

	std::string filename = argv[1];

	std::ifstream input(filename);

	std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(input);

	lexer->Tokenize();

	std::shared_ptr<Parser> parser = std::make_shared<Parser>(lexer);
	parser->Parse();

	auto variables = std::make_shared<Environment<Variable>>();
	auto procedures = std::make_shared<Environment<Procedure>>();
	Interpreter intrpt(parser, variables, procedures);

	std::shared_ptr<ProgramNode> program = std::dynamic_pointer_cast<ProgramNode>(parser->GetRoot());
	std::vector<std::shared_ptr<Value>> values;

	for (size_t i = 0; i < program->Size(); i++)
	{
		auto value = intrpt.Evaluate((*program)[i]);
		values.push_back(value);
	}

	for (auto i = procedures->Begin(); i != procedures->End(); i++)
	{
		std::cout << "Key: " << i->first << ", Value: " << *i->second->GetBody() << std::endl;
	}

	return 0;
}
