#pragma once

#include <algorithm>
#include <fstream>

#include "cpl-parser/token.h"
#include "cpl-basics/string.hpp"
#include "cpl-basics/array.hpp"
#include "cpl-basics/file.hpp"
#include "cpl-basics/allocator/arena.hpp"

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
		Array<Token*> &GetTokens() { return m_tokens; };

    public:
        bool OneOf(const std::vector<TokenType>& types, TokenType needle)
        {
            return std::find(types.begin(), types.end(), needle) != types.end();
        }

    public:
        void SaveState() { m_state = m_arena.Alloc<LexerState>(m_index); }
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
		Token* GetToken();
		Token* SeekToken();
		Token* NextToken();
		Token* PrevToken();
		inline String GetFilename() const { return m_filename; }
	private:
		Array<Token*> m_tokens;
		String m_filename;

	private: // tokenizer
		size_t m_pos;
		String m_code;
		size_t m_line;
		size_t m_col;

	private: // token manager
		size_t m_index;
        LexerState* m_state;
		ArenaAllocator<> m_arena;
	};
}

