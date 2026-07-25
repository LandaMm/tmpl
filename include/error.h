#ifndef ERROR_H
#define ERROR_H
#include <string>
#include <memory>
#include "location.h"
#include "token.h"

namespace Prelude
{
	class ErrorManager final
	{
	private:
		ErrorManager(const ErrorManager &) = delete;
		ErrorManager &operator=(const ErrorManager &) = delete;

	private:
		ErrorManager();

	private:
		~ErrorManager();

	public:
		static ErrorManager &getInstance()
		{
			static ErrorManager instance;
			return instance;
		}

	public:
		void RaiseError(std::string errorMessage, std::string prefix);

	public:
		void NoInputFile();

    private:
        void LogFileLocation(std::string filename, AST::Location loc, std::string prefix);
        void LogFileLocation(std::string filename, AST::Location loc);
        void LogPrefix(std::string prefix);

	public: // Lexer (Tokenizer)
		void UnexpectedCharacter(std::string filename, char ch, size_t line, size_t col);
		void UnexpectedEscapeCharacter(std::string filename, char ch, size_t line, size_t col);
		void UnexpectedEOF(std::string filename, size_t line, size_t col);

	public: // Parser
		void UnexpectedEofWhileToken(std::string filename, AST::TokenType tokenType, size_t line, size_t col);
		void UnexpectedToken(std::string filename, std::shared_ptr<AST::Token> locToken);
		void UnexpectedToken(std::string filename, std::shared_ptr<AST::Token> locToken, std::shared_ptr<AST::Token> gotToken, AST::TokenType expectedTokenType);
		void UnexpectedToken(std::string filename, std::shared_ptr<AST::Token> gotToken, std::string expected);
		void MissingConstantDefinition(std::string filename, std::shared_ptr<AST::Token> token);
        void UnexpectedFnModifier(std::string filename, std::shared_ptr<AST::Token> gotToken, AST::Location loc);

    public: // CliRunner
        void NotEnoughArgs(int expected, int got, bool atLeast);
        void InvalidArgument(std::string arg, std::string message);
        void FailedOpeningFile(std::string path);
        void ProcedureNotFound(std::string name);
	};
}

#endif
