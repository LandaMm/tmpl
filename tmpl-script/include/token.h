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
		SingleArrow,
		At,
		// Keywords
		Require,
		If,
		Else,
		Var,
		Const,
        Return,
        Fn,
        Export,
        Extern,
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
		"'('",
		"')'",
		"'{'",
		"'}'",
		"'['",
		"']'",
		"'+'",
		"'-'",
		"'*'",
		"'/'",
		"'='",
		"'=='",
		"'<'",
		"'>'",
		"'<='",
		"'>='",
		"'?'",
		"':'",
		"'!'",
		"'!='",
		"'&&'",
		"'||'",
		"'|'",
		"'&'",
		"semicolon",
		"->",
		"@",
		// Keywords
		"require",
		"if",
		"else",
		"var",
		"const",
        "return",
        "fn",
        "export",
        "extern",
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
			std::shared_ptr<T> m_value;
		public:
			TypedValueHolder(std::shared_ptr<T> value) : m_value(value) { }
		public:
			inline std::shared_ptr<T> GetValue() const { return m_value; }
		};
	private:
		TokenType m_type;
		std::shared_ptr<ValueHolder> m_value;
        Location m_loc;
	public:
		Token(TokenType type, size_t line, size_t col)
            : m_type(type), m_loc(Location(line, col)), m_value(nullptr) {}
		Token(TokenType type, std::shared_ptr<ValueHolder> value, size_t line, size_t col)
            : m_type(type), m_loc(Location(line, col)), m_value(value) {}
	public:
		inline TokenType GetType() const { return m_type; }
		template<typename T>
		std::shared_ptr<T> GetValue() const {
			if (!m_value) return nullptr;
			std::shared_ptr<TypedValueHolder<T>> holder = std::dynamic_pointer_cast<TypedValueHolder<T>>(m_value);
			return holder ? holder->GetValue() : nullptr;
		}
	public:
		inline size_t GetLine() const { return m_loc.line; }
		inline size_t GetColumn() const { return m_loc.col; }
        inline Location GetLocation() const { return m_loc; }
	public:
		static std::string GetTokenTypeCharacter(TokenType type)
		{
			return TokenTypeCharacters[(size_t)type];
		}
	public:
		friend std::ostream& operator<<(std::ostream& stream, const Token& token);
	};
}

#endif
