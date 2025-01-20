
#include<iostream>
#include"../include/token.h"
#include"../include/lexer.h"
#include"../include/parser.h"

int main()
{
	using namespace AST;
	std::string code = R"(
	print(5)
	if 5 + 5 == 10
		print("hello world!");
	else
		shell.exit(-1)
)";
	std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(code);
	lexer->Tokenize();
	Parser parser(lexer);
	parser.Parse();
	std::cout << "finished parsing" << std::endl;
	return 0;
}
