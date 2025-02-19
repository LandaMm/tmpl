#ifndef LEXER_H
#define LEXER_H
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include "token.h"

namespace AST
{
    struct LexerState
    {
        size_t index;

        LexerState(size_t i) : index(i) {}
    };

	class Lexer
	{
	private:
		std::vector<std::shared_ptr<Token>> m_tokens;
		std::string m_filename;

	private: // tokenizer
		size_t m_pos;
		std::string m_code;
		size_t m_line;
		size_t m_col;

	private: // token manager
		size_t m_index;
        std::shared_ptr<LexerState> m_state;

	public:
		Lexer(std::string code) : m_code(code)
		{
			m_tokens = std::vector<std::shared_ptr<Token>>();
			m_tokens.reserve(10);

			m_index = 0;
			m_pos = 0;
			m_line = 1;
			m_col = 1;
		}
		Lexer(std::ifstream &input, std::string filename);

	public:
		std::vector<std::shared_ptr<Token>> &GetTokens() { return m_tokens; };

    public:
        void SaveState() { m_state = std::make_shared<LexerState>(m_index); }
        void RestoreState();

	public:
		void Tokenize();
		void Id();
		void String();
		void Number();
		void Comment();

	private:
		void HandleCharacter(char ch);

	public:
		std::shared_ptr<Token> GetToken();
		std::shared_ptr<Token> SeekToken();
		std::shared_ptr<Token> NextToken();
		std::shared_ptr<Token> PrevToken();
		inline std::string GetFilename() const { return m_filename; }
	};
}

#endif
