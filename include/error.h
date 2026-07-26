#pragma once

#include <memory>

#include "basics/string.hpp"
#include "location.h"
#include "token.h"

namespace Prelude
{
	class ErrorManager final
	{
	private:
		ErrorManager();
		~ErrorManager();

		ErrorManager(const ErrorManager &) = delete;
		ErrorManager &operator=(const ErrorManager &) = delete;
		ErrorManager(ErrorManager&&) = delete;
		ErrorManager &operator=(ErrorManager&&) = delete;

	public:
		static ErrorManager &getInstance()
		{
			static ErrorManager instance;
			return instance;
		}

	public:
		void RaiseError(String errorMessage, String prefix);

	public:
		void NoInputFile();

    private:
        void LogFileLocation(String filename, AST::Location loc, String prefix);
        void LogFileLocation(String filename, AST::Location loc);
        void LogPrefix(String prefix);

	public: // Lexer (Tokenizer)
		void UnexpectedCharacter(String filename, char ch, size_t line, size_t col);
		void UnexpectedEscapeCharacter(String filename, char ch, size_t line, size_t col);
		void UnexpectedEOF(String filename, size_t line, size_t col);

	public: // Parser
		void UnexpectedEofWhileToken(String filename, AST::TokenType tokenType, size_t line, size_t col);
		void UnexpectedToken(String filename, std::shared_ptr<AST::Token> locToken);
		void UnexpectedToken(String filename, std::shared_ptr<AST::Token> locToken, std::shared_ptr<AST::Token> gotToken, AST::TokenType expectedTokenType);
		void UnexpectedToken(String filename, std::shared_ptr<AST::Token> gotToken, String expected);
		void MissingConstantDefinition(String filename, std::shared_ptr<AST::Token> token);
        void UnexpectedFnModifier(String filename, std::shared_ptr<AST::Token> gotToken, AST::Location loc);

    public: // CliRunner
        void NotEnoughArgs(int expected, int got, bool atLeast);
        void InvalidArgument(String arg, String message);
        void FailedOpeningFile(String path);
        void ProcedureNotFound(String name);
	};
}

