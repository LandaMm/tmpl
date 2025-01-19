#pragma once
#ifndef TOKEN_H
#define TOKEN_H

namespace Compiler
{
	enum class TokenType {
		Id,
		Integer,
		Float,
		Double,
		String,
		Whitespace,
		Newline,
		Point,
		Comma,
		OpenBracket,
		CloseBracket,
		OpenCurly,
		CloseCurly,
		OpenSquareBracket,
		CloseSquareBracket,
		Plus,
		Minus,
		Multiply,
		Divide,
		Equal,
		Compare,
		Less,
		Greater,
		LessEqual,
		GreaterEqual,
		Question,
		Colon,
		Not,
		NotEqual,
		And,
		Or,
		Bind,
		Ampersand,
		Semicolon,
		// Keywords
		Require,
		If,
		Else,
		_EOF,
	};

	class Token
	{
	private:
		TokenType m_type;
		void* m_value;
		size_t m_line;
		size_t m_col;
	public:
		Token(TokenType type, size_t line, size_t col) : m_type(type), m_line(line), m_col(col), m_value(nullptr) {}
		Token(TokenType type, void* value, size_t line, size_t col) : m_type(type), m_line(line), m_col(col), m_value(value) {}
	public:
		inline TokenType GetType() const { return m_type; }
		template<typename T>
		T* GetValue() const { return (T*)m_value; }
		~Token()
		{
			if (m_value != nullptr)
			{
				delete m_value;
			}
		}
	public:
		friend std::ostream& operator<<(std::ostream& stream, const Token& token);
	};
}

#endif