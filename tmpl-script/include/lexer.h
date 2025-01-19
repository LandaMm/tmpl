#pragma once
#ifndef LEXER_H
#define LEXER_H
#include<vector>
#include<string>
#include<memory>
#include"token.h"

namespace Compiler
{
	class Lexer
	{
	private:
		std::vector<std::shared_ptr<Token>> m_tokens;
	private: // tokenizer
		size_t m_pos;
		std::string m_code;
		size_t m_line;
		size_t m_col;
	private: // token manager
		size_t m_index;
	public:
		Lexer(std::string code) : m_code(code), m_pos(0)
		{
			m_tokens = std::vector<std::shared_ptr<Token>>();
			m_tokens.reserve(10);

			m_index = 0;
			m_pos = 0;
			m_line = 1;
			m_col = 0;
		}
	public:
		std::vector<std::shared_ptr<Token>>& GetTokens() { return m_tokens; };
	public:
		void Tokenize();
		void Id();
		void String();
		void Number();
		void Comment();
	public:
		std::shared_ptr<Token> GetToken();
		std::shared_ptr<Token> SeekToken();
		std::shared_ptr<Token> NextToken();
	};
}

#endif
