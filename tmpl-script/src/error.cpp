
#include <cassert>
#include <filesystem>
#include "../include/error.h"

using namespace AST;
namespace fs = std::filesystem;

namespace Prelude
{
	ErrorManager::ErrorManager() {}
	ErrorManager::~ErrorManager() {}
	void ErrorManager::RaiseError(std::string errorMessage)
	{
        std::cerr << "Error: " << errorMessage
				  << std::endl;
		std::exit(-1);
	}
    void ErrorManager::LogFileLocation(std::string filename, Location loc, std::string prefix)
    {
        fs::path cwd = fs::current_path();
        fs::path relative = fs::relative(filename, cwd);
        std::cerr << "\033[90m[" << relative.string() << ":" << loc.line << ":" << loc.col << "]\033[0m \033[1;31m" << prefix << "\033[0m: ";
    }
	void ErrorManager::NoInputFile()
	{
        std::cerr << "Error: No input file provided." << '\n'
				  << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedCharacter(std::string filename, char ch, size_t line, size_t col)
	{
        LogFileLocation(filename, Location(line, col), "LexerError");
        std::cerr << "Unexpected character '" << ch << "' (code: " << (int)ch << ") met" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedEOF(std::string filename, size_t line, size_t col)
	{
        LogFileLocation(filename, Location(line, col), "LexerError");
        std::cerr << "Unexpected end of file while tokenizing" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedEofWhileToken(std::string filename, AST::TokenType tokenType, size_t line, size_t col)
	{
        LogFileLocation(filename, Location(line, col), "ParseError");
        std::cerr << "Expected '" << Token::GetTokenTypeCharacter(tokenType) << "' character but got EOF" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedToken(std::string filename, std::shared_ptr<Token> locToken)
	{
        LogFileLocation(filename, locToken->GetLocation(), "ParseError");
        std::cerr << "Unexpected token '" << Token::GetTokenTypeCharacter(locToken->GetType()) << "' token" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedToken(std::string filename, std::shared_ptr<Token> locToken, std::shared_ptr<Token> gotToken, TokenType expectedTokenType)
	{
        LogFileLocation(filename, locToken->GetLocation(), "ParseError");
        std::cerr << "Expected '" << Token::GetTokenTypeCharacter(expectedTokenType) << "' but got '" << Token::GetTokenTypeCharacter(gotToken->GetType()) << "'" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::MissingConstantDefinition(std::string filename, std::shared_ptr<Token> token)
	{
        LogFileLocation(filename, token->GetLocation(), "ParseError");
        std::cerr << "Expected constant definition "
				  << "but got unexpected '" << Token::GetTokenTypeCharacter(token->GetType()) << "'" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::VarMismatchType(std::string filename, std::string name, Runtime::ValueType type, Runtime::ValueType expectedType, Location loc, std::string prefix)
	{
        // TODO: allow defining double type variable with float value (casting) and opposite direction
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Type mismatch for variable '" << name << "'. Expected type '" << (int)expectedType << "' but got '" << (int)type << "'" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::OperandMismatchType(std::string filename, Runtime::ValueType leftType, Runtime::ValueType rightType, Location loc, std::string prefix)
	{
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Mismatch type of left and right operands '" << (int)leftType << "' != '" << (int)rightType << "'" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UndefinedType(std::string filename, std::string name, Location loc, std::string prefix)
	{
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Undefined type '" << name << "'" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UndeclaredVariable(std::string filename, std::shared_ptr<Nodes::IdentifierNode> id, std::string prefix)
	{
        auto loc = id->GetLocation();
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Undeclared variable '" << id->GetName() << "'" << std::endl;
		std::exit(-1);
	}
    void ErrorManager::ArgMismatchType(std::string filename, std::string name, Runtime::ValueType type, Runtime::ValueType expectedType, Location loc, std::string prefix)
    {
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Argument type ("
            << std::to_string((int)type)
            << ") of parameter '" << name << "' doesn't match parameter type ("
            << std::to_string((int)expectedType)
            << ")" << std::endl;
        exit(-1);
    }

    void ErrorManager::ReturnMismatchType(std::string filename, std::string name, Runtime::ValueType type, Runtime::ValueType expectedType, Location loc)
    {
        LogFileLocation(filename, loc, "RuntimeError");
        std::cerr << "Return value type ("
            << std::to_string((int)type)
            << ") of the function '" << name << "' doesn't match function's signature type ("
            << std::to_string((int)expectedType)
            << ")" << std::endl;
        exit(-1);
    }

    void ErrorManager::UnexpectedReturnType(std::string filename, Runtime::ValueType expected, Runtime::ValueType gotType, Location loc)
    {
        LogFileLocation(filename, loc, "TypeError");
        std::cerr << "Unexpected return type '" << std::to_string((int)gotType) << "' when '" << std::to_string((int)expected) << "' type was expected" << std::endl;
        exit(-1);
    }

    void ErrorManager::TypeMismatch(std::string filename, Runtime::ValueType left, Runtime::ValueType right, Location loc)
    {
        LogFileLocation(filename, loc, "TypeError");
        std::cerr << "Different return types '" << std::to_string((int)left) << "' and '" << std::to_string((int)right) << "'" << std::endl;
        exit(-1);
    }

    void ErrorManager::ArgsParamsExhausted(std::string filename, std::string name, size_t argsSize, size_t paramsSize, Location loc, std::string prefix)
    {
        LogFileLocation(filename, loc, prefix);
        assert(argsSize != paramsSize && "Args count is same as params count. Should be unreachable");
        
        if (argsSize < paramsSize)
        {
            std::cerr << "Exhausted args for function '" << name << "'. Needs to provide " << paramsSize << " arguments but only " << argsSize << " provided";
        }
        else
        {
            std::cerr << "Exhausted params for function '" << name << "'. Needs to provide " << paramsSize << " arguments but " << argsSize << " provided";
        }
        std::cerr << '\n';

        exit(-1);
    }

    void ErrorManager::UnaryOperatorNotSupported(std::string filename, std::string op, Runtime::ValueType metType, Location loc)
    {
        LogFileLocation(filename, loc, "RuntimeError");
        std::cerr << "Unary operator '" << op
            << "' is not allowed with type '" << std::to_string((int)metType) << "'" << std::endl;
        std::exit(-1);
    }

    void ErrorManager::UndeclaredFunction(std::string filename, std::shared_ptr<Nodes::IdentifierNode> id, std::string prefix)
    {
        auto loc = id->GetLocation();
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Calling undeclared function '" << id->GetName() << "'" << std::endl;
		std::exit(-1);
    }

    // CliRunner
    void ErrorManager::NotEnoughArgs(int expected, int got, bool atLeast)
    {
        std::cerr << "ArgumentsError: Not enough additional arguments provided."
            << " Expected " << (atLeast ? "at least" : "") << " " << expected
            << " arguments, but got " << got
            << std::endl;
        std::exit(1);
    }

    void ErrorManager::InvalidArgument(std::string arg, std::string message)
    {
        std::cerr << "ArgumentsError: Invalid argument provided" << arg << "."
            << message << std::endl;
        std::exit(1);
    }

    
    void ErrorManager::FailedOpeningFile(std::string path)
    {
        std::cerr << "FileError: Failed opening file '" << path
            << "'. No such file or directory" << std::endl;
        std::exit(1);
    }

    void ErrorManager::ProcedureNotFound(std::string name)
    {
        std::cerr << "ProcedureError: No procedure found with name '"
            << name << "'" << std::endl;
        std::exit(1);
    }

    void ErrorManager::VarAlreadyExists(std::string filename, std::string name, Location loc, std::string prefix)
    {
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Cannot redeclare already existing variable '"
            << name << "'" << std::endl;
        std::exit(-1);
    }

}
