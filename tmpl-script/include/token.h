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
	public:
		Token(TokenType type) : m_type(type), m_value(nullptr) { }
		Token(TokenType type, void* value) : m_type(type), m_value(value) { }
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