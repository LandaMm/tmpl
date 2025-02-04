
#include <cassert>
#include "../include/error.h"

using namespace AST;

namespace Prelude
{
    // TODO: Refactor error.cpp:
    // 1. create separate function for logging out error location + show relative path to .tmpl file
    // 2. provide error location in all error kinds
    // 3. use stderr instead of stdout
    // 4. colorize error location with grey (like this comment)
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
        // TODO: allow defining double type variable with float value (casting) and opposite direction
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
	void ErrorManager::UndeclaredVariable(std::string filename, std::shared_ptr<Nodes::IdentifierNode> id)
	{
        auto loc = id->GetLocation();
		std::cout << "[" << filename << ":" << loc.line << ":" << loc.col << "] ";
		std::cout << "RuntimeError: Undeclared variable '" << id->GetName() << "'" << std::endl;
		std::exit(-1);
	}
    void ErrorManager::ArgMismatchType(std::string filename, std::string name, Runtime::ValueType type, Runtime::ValueType expectedType, Location loc)
    {
		std::cout << "[" << filename << ":" << loc.line << ":" << loc.col << "] ";
        std::cout << "RuntimeError: Argument type ("
            << std::to_string((int)type)
            << ") of parameter '" << name << "' doesn't match parameter type ("
            << std::to_string((int)expectedType)
            << ")" << std::endl;
        exit(-1);
    }

    void ErrorManager::ReturnMismatchType(std::string filename, std::string name, Runtime::ValueType type, Runtime::ValueType expectedType, Location loc)
    {
		std::cout << "[" << filename << ":" << loc.line << ":" << loc.col << "] ";
        std::cout << "RuntimeError: Return value type ("
            << std::to_string((int)type)
            << ") of the function '" << name << "' doesn't match function's signature type ("
            << std::to_string((int)expectedType)
            << ")" << std::endl;
        exit(-1);
    }

    void ErrorManager::ArgsParamsExhausted(std::string filename, std::string name, size_t argsSize, size_t paramsSize, Location loc)
    {
		std::cout << "[" << filename << ":" << loc.line << ":" << loc.col << "] ";
        std::cout << "RuntimeError: ";

        assert(argsSize != paramsSize && "Args count is same as params count. Should be unreachable");
        
        if (argsSize < paramsSize)
        {
            std::cout << "Exhausted args for function '" << name << "'. Needs to provide " << paramsSize << " arguments but only " << argsSize << " provided";
        }
        else
        {
            std::cout << "Exhausted params for function '" << name << "'. Needs to provide " << paramsSize << " arguments but " << argsSize << " provided";
        }
        std::cout << '\n';

        exit(-1);
    }

    void ErrorManager::UnaryOperatorNotSupported(std::string filename, std::string op, Runtime::ValueType metType, Location loc)
    {
        std::cout << "[" << filename << ":" << loc.line << ":" << loc.col << "] ";
        std::cout << "RuntimeError: Unary operator '" << op
            << "' is not allowed with type '" << std::to_string((int)metType) << "'" << std::endl;
        std::exit(-1);
    }

    void ErrorManager::UndeclaredFunction(std::string filename, std::shared_ptr<Nodes::IdentifierNode> id)
    {
        auto loc = id->GetLocation();
		std::cout << "[" << filename << ":" << loc.line << ":" << loc.col << "] ";
		std::cout << "RuntimeError: Calling undeclared function '" << id->GetName() << "'" << std::endl;
		std::exit(-1);
    }

    // CliRunner
    void ErrorManager::NotEnoughArgs(int expected, int got, bool atLeast)
    {
        std::cout << "ArgumentsError: Not enough additional arguments provided."
            << " Expected " << (atLeast ? "at least" : "") << " " << expected
            << " arguments, but got " << got
            << std::endl;
        std::exit(1);
    }

    void ErrorManager::InvalidArgument(std::string arg, std::string message)
    {
        std::cout << "ArgumentsError: Invalid argument provided" << arg << "."
            << message << std::endl;
        std::exit(1);
    }

    
    void ErrorManager::FailedOpeningFile(std::string path)
    {
        std::cout << "FileError: Failed opening file '" << path
            << "'. No such file or directory" << std::endl;
        std::exit(1);
    }

    void ErrorManager::ProcedureNotFound(std::string name)
    {
        std::cout << "ProcedureError: No procedure found with name '"
            << name << "'" << std::endl;
        std::exit(1);
    }

    void ErrorManager::VarAlreadyExists(std::string filename, std::string name, Location loc)
    {
		std::cout << "[" << filename << ":" << loc.line << ":" << loc.col << "] ";
        std::cout << "RuntimeError: Cannot redeclare already existing variable '"
            << name << "'" << std::endl;
        std::exit(-1);
    }

}
