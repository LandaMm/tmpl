
#include <cassert>
#include <filesystem>
#include <memory>
#include "../include/error.h"
#include "include/location.h"

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
    void ErrorManager::UnexpectedEscapeCharacter(std::string filename, char ch, size_t line, size_t col)
	{
        LogFileLocation(filename, Location(line, col), "LexerError");
        std::cerr << "Unexpected escape character '" << ch << "' (code: " << (int)ch << ") met" << std::endl;
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
    void ErrorManager::UnexpectedToken(std::string filename, std::shared_ptr<AST::Token> gotToken, std::string expected)
    {
        LogFileLocation(filename, gotToken->GetLocation(), "ParseError");
        std::cerr << "Expected " << expected << " but got '" << Token::GetTokenTypeCharacter(gotToken->GetType()) << "'" << std::endl;
		std::exit(-1);
    }
    void ErrorManager::UnexpectedFnModifier(std::string filename, std::shared_ptr<AST::Token> gotToken, AST::Location loc)
    {
        LogFileLocation(filename, loc, "ParseError");
        std::cerr << "Unexpected token used for fn modifier '"
            << Token::GetTokenTypeCharacter(gotToken->GetType()) << "'" << std::endl;
		std::exit(-1);
    }
	void ErrorManager::MissingConstantDefinition(std::string filename, std::shared_ptr<Token> token)
	{
        LogFileLocation(filename, token->GetLocation(), "ParseError");
        std::cerr << "Expected constant definition "
				  << "but got unexpected '" << Token::GetTokenTypeCharacter(token->GetType()) << "'" << std::endl;
		std::exit(-1);
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
}
