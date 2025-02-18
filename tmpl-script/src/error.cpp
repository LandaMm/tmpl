
#include <cassert>
#include <filesystem>
#include <memory>
#include "../include/error.h"
#include "include/interpreter/value.h"

using namespace AST;
namespace fs = std::filesystem;

namespace Prelude
{
	ErrorManager::ErrorManager() {}
	ErrorManager::~ErrorManager() {}
	void ErrorManager::RaiseError(std::string errorMessage, std::string prefix)
	{
        LogPrefix(prefix);
        std::cerr << errorMessage
				  << std::endl;
        if (prefix != "TypeError") std::exit(-1);
	}
    void ErrorManager::LogFileLocation(std::string filename, Location loc, std::string prefix)
    {
        fs::path cwd = fs::current_path();
        fs::path relative = fs::relative(filename, cwd);
        std::cerr << "\033[90m[" << relative.string() << ":" << loc.line << ":" << loc.col << "]\033[0m \033[1;31m" << prefix << "\033[0m: ";
    }

    void ErrorManager::LogFileLocation(std::string filename, Location loc)
    {
        fs::path cwd = fs::current_path();
        fs::path relative = fs::relative(filename, cwd);
        std::cerr << "(\033[90m" << relative.string() << ":" << loc.line << ":" << loc.col << "\033[0m)";
    }

    void ErrorManager::LogPrefix(std::string prefix)
    {
        std::cerr << "\033[1;31m" << prefix << "\033[0m: ";
    }

	void ErrorManager::NoInputFile()
	{
        LogPrefix("Error");
        std::cerr << "No input file provided." << '\n'
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
	void ErrorManager::VarMismatchType(std::string filename, std::string name, Runtime::PValType type, Runtime::PValType expectedType, Location loc, std::string prefix)
	{
        // TODO: allow defining double type variable with float value (casting) and opposite direction
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Type mismatch for variable '" << name
            << "'. Expected type '" << *expectedType
            << "' but got '" << *type << "'" << std::endl;
        if (prefix != "TypeError") std::exit(-1);
	}
	void ErrorManager::OperandMismatchType(std::string filename, Runtime::PValType leftType, Runtime::PValType rightType, Location loc, std::string prefix)
	{
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Mismatch type of left and right operands '" << *leftType << "' != '" << *rightType << "'" << std::endl;
        if (prefix != "TypeError") std::exit(-1);
	}
	void ErrorManager::UndefinedType(std::string filename, std::string name, Location loc, std::string prefix)
	{
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Undefined type '" << name << "'" << std::endl;
        if (prefix != "TypeError") std::exit(-1);
	}
	void ErrorManager::UndeclaredVariable(std::string filename, std::shared_ptr<Nodes::IdentifierNode> id, std::string prefix)
	{
        auto loc = id->GetLocation();
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Undeclared variable '" << id->GetName() << "'" << std::endl;
        if (prefix != "TypeError") std::exit(-1);
	}
    void ErrorManager::ArgMismatchType(std::string filename, std::string name, Runtime::PValType type, Runtime::PValType expectedType, Location loc, std::string prefix)
    {
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Argument type '"
            << *type
            << "' of parameter '" << name << "' doesn't match parameter type '"
            << *expectedType
            << "'" << std::endl;
        if (prefix != "TypeError") exit(-1);
    }

    void ErrorManager::ReturnMismatchType(std::string filename, std::string name, Runtime::PValType type, Runtime::PValType expectedType, Location loc)
    {
        LogFileLocation(filename, loc, "RuntimeError");
        std::cerr << "Return value type '"
            << *type
            << "' of the function '" << name << "' doesn't match function's signature type '"
            << *expectedType
            << "'" << std::endl;
        exit(-1);
    }

    void ErrorManager::UnexpectedReturnType(std::string filename, Runtime::PValType expected, Runtime::PValType gotType, Location loc)
    {
        LogFileLocation(filename, loc, "TypeError");
        std::cerr << "Unexpected return type '" << *gotType << "' when '" << *expected << "' type was expected" << std::endl;
        /*exit(-1);*/
    }

    void ErrorManager::TypeMismatch(std::string filename, Runtime::PValType left, Runtime::PValType right, Location loc)
    {
        LogFileLocation(filename, loc, "TypeError");
        std::cerr << "Different return types '" << *left
            << "' and '" << *right << "'" << std::endl;
        /*exit(-1);*/
    }

    void ErrorManager::TypeDoesNotExist(std::string filename, Runtime::PValType typ, AST::Location loc, std::string prefix)
    {
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Type '" << *typ << "' is not defined" << std::endl;
        if (prefix != "TypeError") exit(-1);
    }

    void ErrorManager::TypeCastNotPossible(std::string filename, Runtime::PValType from, Runtime::PValType to, AST::Location loc, std::string prefix)
    {
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Type '" << *from << "' cannot be converted to type '"
            << *to << "'" << std::endl;
        if (prefix != "TypeError") exit(-1);
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

        if (prefix != "TypeError") exit(-1);
    }

    void ErrorManager::UnaryOperatorNotSupported(std::string filename, std::string op, Runtime::PValType metType, Location loc)
    {
        LogFileLocation(filename, loc, "RuntimeError");
        std::cerr << "Unary operator '" << op
            << "' is not allowed with type '" << *metType << "'" << std::endl;
        std::exit(-1);
    }

    void ErrorManager::UndeclaredFunction(std::string filename, std::shared_ptr<Nodes::IdentifierNode> id, std::string prefix)
    {
        auto loc = id->GetLocation();
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Calling undeclared function '" << id->GetName() << "'" << std::endl;
        if (prefix != "TypeError") std::exit(-1);
    }

    void ErrorManager::UndeclaredFunction(std::string filename, std::shared_ptr<AST::Nodes::ObjectMember> obj, Runtime::PValType valType, std::string prefix)
    {
        auto loc = obj->GetLocation();
        LogFileLocation(filename, loc, prefix);
        assert(obj->GetMember()->GetType() == NodeType::Identifier && "Object member for fn call should be an identifier.");
        auto id = std::dynamic_pointer_cast<Nodes::IdentifierNode>(obj->GetMember());
        std::cerr << "Calling undeclared function '" << id->GetName() << "' for type '" << *valType << "'" << std::endl;
        if (prefix != "TypeError") std::exit(-1);
    }

    // CliRunner
    void ErrorManager::NotEnoughArgs(int expected, int got, bool atLeast)
    {
        LogPrefix("ArgumentsError");
        std::cerr << "Not enough additional arguments provided."
            << " Expected " << (atLeast ? "at least" : "") << " " << expected
            << " arguments, but got " << got
            << std::endl;
        std::exit(1);
    }

    void ErrorManager::InvalidArgument(std::string arg, std::string message)
    {
        LogPrefix("ArgumentsError");
        std::cerr << "Invalid argument provided" << arg << "."
            << message << std::endl;
        std::exit(1);
    }

    
    void ErrorManager::FailedOpeningFile(std::string path)
    {
        LogPrefix("FileError");
        std::cerr << "Failed opening file '" << path
            << "'. No such file or directory" << std::endl;
        std::exit(1);
    }

    void ErrorManager::ProcedureNotFound(std::string name)
    {
        LogPrefix("ProcedureError");
        std::cerr << "No procedure found with name '"
            << name << "'" << std::endl;
        std::exit(1);
    }

    void ErrorManager::VarAlreadyExists(std::string filename, std::string name, Location loc, std::string prefix)
    {
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Cannot redeclare already existing variable '"
            << name << "'" << std::endl;
        if (prefix != "TypeError") std::exit(-1);
    }

        void ErrorManager::FunctionRedeclaration(std::string filename, std::string name, AST::Location loc, std::string declFilename, AST::Location declLoc, std::string prefix)
        {
            LogFileLocation(filename, loc, prefix);
            std::cerr << "Cannot redeclare already existing function '"
                << name << "' at ";
            LogFileLocation(declFilename, declLoc);
            std::cerr << std::endl;
            if (prefix != "TypeError") std::exit(-1);
        }

    void ErrorManager::PrivateFunctionError(std::string filename, std::string fnName, std::string fnModule, Location loc, Location mLoc, std::string prefix)
    {
        LogFileLocation(filename, loc, prefix);
        std::cerr << "Cannot call a private function '" << fnName
            << "' outside of it's module ";
        LogFileLocation(fnModule, mLoc);
        std::cerr << std::endl;
        if (prefix != "TypeError") std::exit(-1);
    }

}
