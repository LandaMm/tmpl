#pragma once
#ifndef ERROR_H
#define ERROR_H
#include<iostream>
#include<string>
#include<memory>
#include"token.h"
#include"interpreter/value.h"
#include"node/identifier.h"

using namespace AST;

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
		void MissingConstantDefinition(std::shared_ptr<Token> token);
	public: // Interpreter
		void VarMismatchType(std::string name, Runtime::ValueType type, Runtime::ValueType expectedType);
		void UndefinedType(std::string name);
		void UndeclaredVariable(std::shared_ptr<Nodes::IdentifierNode> id);
		void OperandMismatchType(Runtime::ValueType leftType, Runtime::ValueType rightType);
	};
}

#endif
