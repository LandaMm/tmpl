
#include "../include/error.h"

using namespace AST;

namespace Prelude
{
	ErrorManager::ErrorManager() {}
	ErrorManager::~ErrorManager() {}
	void ErrorManager::RaiseError(std::string errorMessage)
	{
		std::cout << "Error: " << errorMessage
				  << std::endl;
		std::exit(-1);
	}
	void ErrorManager::NoInputFile()
	{
		std::cout << "Error: No input file provided." << '\n'
				  << std::endl;
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
		std::cout << "ParseError: Expected '" << Token::GetTokenTypeCharacter(tokenType) << "' character but got eof at line " << line << " and " << col << " column." << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedToken(std::string filename, std::shared_ptr<Token> locToken)
	{
		std::cout << "[" << filename << ":" << locToken->GetLine() << ":" << locToken->GetColumn() << "] ";
		std::cout << "ParseError: Unexpected token '" << Token::GetTokenTypeCharacter(locToken->GetType()) << "' token" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedToken(std::string filename, std::shared_ptr<Token> locToken, std::shared_ptr<Token> gotToken, TokenType expectedTokenType)
	{
		std::cout << "[" << filename << ":" << locToken->GetLine() << ":" << locToken->GetColumn() << "] ";
		std::cout << "ParseError: Expected '" << Token::GetTokenTypeCharacter(expectedTokenType) << "' but got '" << Token::GetTokenTypeCharacter(gotToken->GetType()) << "'" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::MissingConstantDefinition(std::shared_ptr<Token> token)
	{
		std::cout << "ParseError: Expected constant definition at line "
				  << token->GetLine() << " and " << token->GetColumn() << " column but got unexpected '" << Token::GetTokenTypeCharacter(token->GetType()) << "'" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::VarMismatchType(std::string name, Runtime::ValueType type, Runtime::ValueType expectedType)
	{
		std::cout << "RuntimeError: Type mismatch for variable '" << name << "'. Expected type '" << (int)expectedType << "' but got '" << (int)type << "'" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::OperandMismatchType(Runtime::ValueType leftType, Runtime::ValueType rightType)
	{
		std::cout << "RuntimeError: Mismatch type of left and right operands '" << (int)leftType << "' != '" << (int)rightType << "'" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UndefinedType(std::string name)
	{
		std::cout << "RuntimeError: Undefined type '" << name << "'" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UndeclaredVariable(std::shared_ptr<Nodes::IdentifierNode> id)
	{
		std::cout << "RuntimeError: Undeclared variable '" << id->GetName() << "'" << std::endl;
		std::exit(-1);
	}
}
