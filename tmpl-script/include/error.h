#pragma once
#ifndef ERROR_H
#define ERROR_H
#include<iostream>
#include<string>
#include"token.h"

using namespace Compiler;

namespace Prelude
{
	class ErrorManager final
	{
	private:
		ErrorManager(const ErrorManager&) = delete;
		ErrorManager& operator=(const ErrorManager&) = delete;
	private:
		ErrorManager();
	private:
		~ErrorManager();
	public:
		static ErrorManager& getInstance()
		{
			static ErrorManager instance;
			return instance;
		}
	public:
		void RaiseError(std::string errorMessage);
	public: // Lexer (Tokenizer)
		void UnexpectedCharacter(char ch, size_t line, size_t col);
		void UnexpectedEOF(size_t line, size_t col);
	public: // Parser
		void UnexpectedEofWhileToken(TokenType tokenType, size_t line, size_t col);
		void UnexpectedToken(std::shared_ptr<Token> token);
		void UnexpectedToken(std::shared_ptr<Token> token, TokenType expectedTokenType);
	};
}

#endif