#pragma once

#include <memory>

#include "cpl-basics/string.hpp"
#include "cpl-parser/location.h"
#include "cpl-parser/token.h"

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
        void LogCodePiece(String filename, AST::LocationSpan loc);
        void LogFileLocation(String filename, AST::LocationSpan loc, String prefix);
        void LogFileLocation(String filename, AST::LocationSpan loc);
        void LogPrefix(String prefix);

	public: // Lexer (Tokenizer)
		void UnexpectedCharacter(String filename, char ch, size_t line, size_t col);
		void UnexpectedEscapeCharacter(String filename, char ch, size_t line, size_t col);
		void UnexpectedEOF(String filename, size_t line, size_t col);

	public: // Parser
		void UnexpectedEofWhileToken(String filename, AST::TokenType tokenType, size_t line, size_t col);
		void UnexpectedToken(String filename, AST::Token* locToken);
		void UnexpectedToken(String filename, AST::Token* locToken, AST::Token* gotToken, AST::TokenType expectedTokenType);
		void UnexpectedToken(String filename, AST::Token* gotToken, String expected);
		void MissingConstantDefinition(String filename, AST::Token* token);
        void UnexpectedFnModifier(String filename, AST::Token* gotToken, AST::LocationSpan loc);

    public: // CliRunner
        void NotEnoughArgs(int expected, int got, bool atLeast);
        void InvalidArgument(String arg, String message);
        void FailedOpeningFile(String path);
        void ProcedureNotFound(String name);
	};
}

