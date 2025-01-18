
#include<iostream>
#include"../include/token.h"
#include"../include/lexer.h"
#include"../include/parser.h"

int main()
{
	using namespace Compiler;
	std::string code = "if 5 == 5 { print(true) } else print(false)";
	std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(code);
	lexer->Tokenize();
	std::vector<std::shared_ptr<Token>>& tokens = lexer->GetTokens();
	std::cout << "tokenized code and got " << tokens.size() << std::endl;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		std::shared_ptr<Token> token = tokens[i];
		std::cout << *token << std::endl;
	}
	Parser parser(lexer);
	parser.Parse();
	std::cout << "finished parsing" << std::endl;
	return 0;
}
