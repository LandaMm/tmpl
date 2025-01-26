
#include <iostream>
#include "../include/token.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interpreter.h"
#include "../include/interpreter/environment.h"

int main()
{
	using namespace AST;
	using namespace Runtime;
	/*std::string code = R"(
	const string test = "Hello";
	const string hw = test + ", world!";
	hw;
	5478.0 * (2375.0 / 2200.0 + 1.0) - 5000.0;
)";*/
	std::string code = R"(
	10;
	var string result = 1 + 1 != 1 ? "hell yeah!" : "whaat?";
	result;
)";
	std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(code);
	lexer->Tokenize();
	std::shared_ptr<Parser> parser = std::make_shared<Parser>(lexer);
	parser->Parse();
	std::cout << "finished parsing" << std::endl;
	std::shared_ptr<Environment> env = std::make_shared<Environment>();
	Interpreter intrpt(parser, env);
	std::shared_ptr<ProgramNode> program = std::dynamic_pointer_cast<ProgramNode>(parser->GetRoot());
	std::vector<std::shared_ptr<Value>> values;
	for (size_t i = 0; i < program->Size(); i++)
	{
		auto value = intrpt.Evaluate((*program)[i]);
		std::cout << "statement #" << i + 1 << " evaluated." << std::endl;
		std::cout << "value: " << value << std::endl;
		values.push_back(value);
	}
	std::cout << "finished interpreting" << std::endl;
	return 0;
}
