#ifndef TOKEN_H
#define TOKEN_H
#include<vector>
#include<memory>
#include "location.h"

namespace AST
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
        CompoundAdd,
        CompoundMinus,
        CompoundMultiply,
        CompoundDivide,
		Compare,
		Less,
		Greater,
		LessEqual,
		GreaterEqual,
		Question,
		Colon,
		DoubleColon,
		ColonEqual,
		Not,
		NotEqual,
		And,
		Or,
		Bind,
		Ampersand,
		Semicolon,
		SingleArrow,
		At,
		Hash,
		// Keywords
		Import,
		Foreign,
		If,
		Else,
		Var,
		Const,
        Return,
        Extern,
        True,
        False,
        TypeDf,
        New,
        While,
        For,
        Break,
		_EOF,
	};

	static const std::vector<std::string> TokenTypeCharacters = {
		"identifier",
		"integer",
		"float",
		"double",
		"string",
		"whitespace",
		"newline",
		"point",
		"comma",
		"(",
		")",
		"{",
		"}",
		"[",
		"]",
		"+",
		"-",
		"*",
		"/",
		"=",
        "+=",
        "-=",
        "*=",
        "/=",
		"==",
		"<",
		">",
		"<=",
		">=",
		"?",
		":",
		"::",
		":=",
		"!",
		"!=",
		"&&",
		"||",
		"|",
		"&",
		"semicolon",
		"->",
		"@",
		"#",
		// Keywords
		"import",
		"foreign",
		"if",
		"else",
		"var",
		"const",
        "return",
        "extern",
        "true",
        "false",
        "typedf",
        "new",
        "while",
        "for",
        "break",
		"EOF",
	};

	class Token
	{
	public:
		class ValueHolder
		{
		public:
			virtual ~ValueHolder() = default;
		};

		template<typename T>
		class TypedValueHolder : public ValueHolder
		{
		private:
			T* m_value;
		public:
			TypedValueHolder(T* value) : m_value(value) { }
		public:
			inline T* GetValue() const { return m_value; }
		};
	public:
		Token(TokenType type, Location begin, Location end)
            : m_type(type), m_loc(begin, end), m_value(nullptr) {}
		Token(TokenType type, ValueHolder* value, Location begin, Location end)
            : m_type(type), m_loc(begin, end), m_value(value) {}
		~Token() = default;

		Token(const Token&) = delete;
		Token& operator=(const Token&) = delete;
		Token(Token&&) = delete;
		Token& operator=(Token&&) = delete;
	public:
		inline TokenType GetType() const { return m_type; }
		template<typename T>
		T* GetValue() const {
			if (!m_value) return nullptr;
			TypedValueHolder<T>* holder = reinterpret_cast<TypedValueHolder<T>*>(m_value);
			return holder ? holder->GetValue() : nullptr;
		}
	public:
		inline size_t GetLine() const { return m_loc.begin.line; }
		inline size_t GetColumn() const { return m_loc.end.col; }
        inline LocationSpan GetLocation() const { return m_loc; }
	public:
		static std::string GetTokenTypeCharacter(TokenType type)
		{
			return TokenTypeCharacters[(size_t)type];
		}
	public:
		friend std::ostream& operator<<(std::ostream& stream, const Token& token);
	private:
		TokenType m_type;
		ValueHolder* m_value;
        LocationSpan m_loc;
	};
}

#endif
