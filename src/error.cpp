
#include <cstdio>
#include <cassert>
#include <filesystem>
#include <memory>

#include "basics/file.hpp"
#include "error.h"
#include "location.h"
#include "basics/string.hpp"

using namespace AST;
namespace fs = std::filesystem;

namespace Prelude
{
	ErrorManager::ErrorManager() {}
	ErrorManager::~ErrorManager() {}
	void ErrorManager::RaiseError(String errorMessage, String prefix)
	{
        LogPrefix(prefix);
        std::cerr << errorMessage
				  << std::endl;
        if (prefix != "TypeError") std::exit(-1);
	}

    void ErrorManager::LogCodePiece(String filename, AST::LocationSpan loc)
    {
        FileReader reader(filename.c_str());
        auto content = reader.ReadEntireText();
        Uint32 lineIndex = 1;
        Uint32 lineStart = 0;
        Uint32 lookingForLine = loc.begin.line > 1 ? loc.begin.line - 1 : loc.begin.line;
        for (Uint32 i = 0; i < content.Size(); ++i)
        {
            if (content[i] == '\n')
            {
				lineIndex++;
                lineStart = i + 1;
            }
            if (lineIndex == lookingForLine) break;
        }

        // log previous line as well for better navigation
        Uint32 requiredLineNumberWidth = std::to_string(loc.begin.line + 1).size() + 1;
        for (Uint32 i = 0; i < 2; ++i)
        {
			String line = content.Substr(lineStart, content.Substr(lineStart).Find('\n'));
			std::cerr << "\033[90m";
            std::fprintf(stderr, "% *d", requiredLineNumberWidth, lookingForLine + i);
			std::cerr << "\033[0m";
            std::cerr << " " << line << std::endl;
            lineStart += line.Size() + 1;
        }
		std::cerr << std::endl;
        
        Uint32 padding = loc.begin.col + requiredLineNumberWidth + 1;
        Uint32 length = loc.end.col - loc.begin.col;

        Array<char> pad(padding);
        pad.ResizeUninitialized(padding);
        std::memset(pad.Data(), ' ', padding);
        pad.Push('\0');

        Array<char> pointer(length);
        pointer.ResizeUninitialized(length);
        std::memset(pointer.Data(), '^', length);
        pointer.Push('\0');

        std::cerr << pad.Data() << pointer.Data() << std::endl << std::endl;
	}

    void ErrorManager::LogFileLocation(String filename, LocationSpan loc, String prefix)
    {
        fs::path cwd = fs::current_path();
        fs::path relative = fs::relative(filename.c_str(), cwd);

        LogCodePiece(filename, loc);

        std::cerr << "\033[90m[" << relative.string() << ":" << loc.begin.line << ":" << loc.begin.col << "]\033[0m \033[1;31m" << prefix << "\033[0m: ";
    }

    void ErrorManager::LogFileLocation(String filename, LocationSpan loc)
    {
        fs::path cwd = fs::current_path();
        fs::path relative = fs::relative(filename.c_str(), cwd);

        LogCodePiece(filename, loc);

        std::cerr << "(\033[90m" << relative.string() << ":" << loc.begin.line << ":" << loc.begin.col << "\033[0m)";
    }

    void ErrorManager::LogPrefix(String prefix)
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
	void ErrorManager::UnexpectedCharacter(String filename, char ch, size_t line, size_t col)
	{
        auto loc = Location( line, col );
        LogFileLocation(filename, LocationSpan{ loc, loc }, "LexerError");
        std::cerr << "Unexpected character '" << ch << "' (code: " << (int)ch << ") met" << std::endl;
		std::exit(-1);
	}
    void ErrorManager::UnexpectedEscapeCharacter(String filename, char ch, size_t line, size_t col)
	{
        auto loc = Location( line, col );
        LogFileLocation(filename, LocationSpan { loc, loc }, "LexerError");
        std::cerr << "Unexpected escape character '" << ch << "' (code: " << (int)ch << ") met" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedEOF(String filename, size_t line, size_t col)
	{
        auto loc = Location( line, col );
        LogFileLocation(filename, LocationSpan { loc, loc }, "LexerError");
        std::cerr << "Unexpected end of file while tokenizing" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedEofWhileToken(String filename, AST::TokenType tokenType, size_t line, size_t col)
	{
        auto loc = Location( line, col );
        LogFileLocation(filename, LocationSpan{loc, loc}, "ParseError");
        std::cerr << "Expected '" << Token::GetTokenTypeCharacter(tokenType) << "' character but got EOF" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedToken(String filename, Token* locToken)
	{
        LogFileLocation(filename, locToken->GetLocation(), "ParseError");
        std::cerr << "Unexpected token '" << Token::GetTokenTypeCharacter(locToken->GetType()) << "' token" << std::endl;
		std::exit(-1);
	}
	void ErrorManager::UnexpectedToken(String filename, Token* locToken, Token* gotToken, TokenType expectedTokenType)
	{
        LogFileLocation(filename, locToken->GetLocation(), "ParseError");
        std::cerr << "Expected '" << Token::GetTokenTypeCharacter(expectedTokenType) << "' but got '" << Token::GetTokenTypeCharacter(gotToken->GetType()) << "'" << std::endl;
		std::exit(-1);
	}
    void ErrorManager::UnexpectedToken(String filename, AST::Token* gotToken, String expected)
    {
        LogFileLocation(filename, gotToken->GetLocation(), "ParseError");
        std::cerr << "Expected " << expected << " but got '" << Token::GetTokenTypeCharacter(gotToken->GetType()) << "'" << std::endl;
		std::exit(-1);
    }
    void ErrorManager::UnexpectedFnModifier(String filename, AST::Token* gotToken, AST::LocationSpan loc)
    {
        LogFileLocation(filename, loc, "ParseError");
        std::cerr << "Unexpected token used for fn modifier '"
            << Token::GetTokenTypeCharacter(gotToken->GetType()) << "'" << std::endl;
		std::exit(-1);
    }
	void ErrorManager::MissingConstantDefinition(String filename, Token* token)
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

    void ErrorManager::InvalidArgument(String arg, String message)
    {
        LogPrefix("ArgumentsError");
        std::cerr << "Invalid argument provided" << arg << "."
            << message << std::endl;
        std::exit(1);
    }


    void ErrorManager::FailedOpeningFile(String path)
    {
        LogPrefix("FileError");
        std::cerr << "Failed opening file '" << path
            << "'. No such file or directory" << std::endl;
        std::exit(1);
    }

    void ErrorManager::ProcedureNotFound(String name)
    {
        LogPrefix("ProcedureError");
        std::cerr << "No procedure found with name '"
            << name << "'" << std::endl;
        std::exit(1);
    }
}
