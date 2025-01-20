
#include"../include/error.h"

using namespace AST;

namespace Prelude
{
	ErrorManager::ErrorManager() { }
	ErrorManager::~ErrorManager() { }
	void ErrorManager::RaiseError(std::string errorMessage)
	{
		std::cout << "Error: " << errorMessage << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedCharacter(char ch, size_t line, size_t col)
	{
		std::cout << "LexerError: Unexpected character '" << ch << "' (code: " << (int)ch << ") met at line " << line << " and " << col << " column." << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedEOF(size_t line, size_t col)
	{
		std::cout << "LexerError: Unexpected end of file while tokenizing at line " << line << " and " << col << " column." << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedEofWhileToken(AST::TokenType tokenType, size_t line, size_t col)
	{
		// TODO: show character by token type instead of token type itself
		std::cout << "ParseError: Expected '" << Token::GetTokenTypeCharacter(tokenType) <<"' character but got eof at line " << line << " and " << col << " column." << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedToken(std::shared_ptr<Token> token)
	{
		// TODO: show character by token type instead of token type itself
		std::cout << "ParseError: Unexpected token '" << Token::GetTokenTypeCharacter(token->GetType()) << "' at line "
			<< token->GetLine() << " and " << token->GetColumn() << " column." << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedToken(std::shared_ptr<Token> token, TokenType expectedTokenType)
	{
		// TODO: show character by token type instead of token type itself
		std::cout << "ParseError: Expected '" << Token::GetTokenTypeCharacter(expectedTokenType) << "' at line "
			<< token->GetLine() << " and " << token->GetColumn() << " column but got '" << Token::GetTokenTypeCharacter(token->GetType()) << "'" << std::endl;
		std::exit(-1);
	}
}
