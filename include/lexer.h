#pragma once

#include <algorithm>
#include <memory>
#include <fstream>

#include "token.h"
#include "basics/string.hpp"
#include "basics/array.hpp"
#include "basics/file.hpp"

namespace AST
{
    struct LexerState
    {
        size_t index;

        LexerState(size_t i) : index(i) {}
    };

	class Lexer
	{
	public:
		Lexer(String code);
		Lexer(FileReader& reader, String filename);

	public:
		Array<std::shared_ptr<Token>> &GetTokens() { return m_tokens; };

    public:
        bool OneOf(const std::vector<TokenType>& types, TokenType needle)
        {
            return std::find(types.begin(), types.end(), needle) != types.end();
        }

    public:
        void SaveState() { m_state = std::make_shared<LexerState>(m_index); }
        void RestoreState();

	public:
		void Tokenize();
		void Id();
		void StringLiteral();
		void Number();
		void Comment();

	private:
		void HandleCharacter(char ch);

	public:
		std::shared_ptr<Token> GetToken();
		std::shared_ptr<Token> SeekToken();
		std::shared_ptr<Token> NextToken();
		std::shared_ptr<Token> PrevToken();
		inline String GetFilename() const { return m_filename; }
	private:
		Array<std::shared_ptr<Token>> m_tokens;
		String m_filename;

	private: // tokenizer
		size_t m_pos;
		String m_code;
		size_t m_line;
		size_t m_col;

	private: // token manager
		size_t m_index;
        std::shared_ptr<LexerState> m_state;
	};
}

