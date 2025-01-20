
#include"../include/error.h"
#include"../include/lexer.h"

namespace AST
{
	void Lexer::Tokenize()
	{
		while (m_pos < m_code.size())
		{
			char ch = m_code[m_pos];

			if (isalpha(ch))
			{
				Id();
				continue;
			}

			if (isdigit(ch))
			{
				Number();
				continue;
			}

			if (ch == '"')
			{
				String();
				continue;
			}

			if (ch == '#')
			{
				// skip comment symbol
				m_pos++;
				Comment();
				continue;
			}

			switch (ch)
			{
			case '\n':
				// skip newline
				//m_tokens.push_back(std::make_shared<Token>(TokenType::Newline));
				break;
			case ' ':
			case '\t':
			case '\r':
				// skip those characters
				//m_tokens.push_back(std::make_shared<Token>(TokenType::Whitespace));
				break;
			case '.':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Point, m_line, m_col));
				break;
			case ',':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Comma, m_line, m_col));
				break;
			case '(':
				m_tokens.push_back(std::make_shared<Token>(TokenType::OpenBracket, m_line, m_col));
				break;
			case ')':
				m_tokens.push_back(std::make_shared<Token>(TokenType::CloseBracket, m_line, m_col));
				break;
			case '{':
				m_tokens.push_back(std::make_shared<Token>(TokenType::OpenCurly, m_line, m_col));
				break;
			case '}':
				m_tokens.push_back(std::make_shared<Token>(TokenType::CloseCurly, m_line, m_col));
				break;
			case '[':
				m_tokens.push_back(std::make_shared<Token>(TokenType::OpenSquareBracket, m_line, m_col));
				break;
			case ']':
				m_tokens.push_back(std::make_shared<Token>(TokenType::CloseSquareBracket, m_line, m_col));
				break;
			case '+':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Plus, m_line, m_col));
				break;
			case '-':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Minus, m_line, m_col));
				break;
			case '*':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Multiply, m_line, m_col));
				break;
			case '/':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Divide, m_line, m_col));
				break;
			case ';':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Semicolon, m_line, m_col));
				break;
			case '=':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::Compare, m_line, m_col));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Equal, m_line, m_col));
				break;
			case '!':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::NotEqual, m_line, m_col));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Not, m_line, m_col));
				break;
			case '&':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '&')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::And, m_line, m_col));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Ampersand, m_line, m_col));
				break;
			case '|':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '|')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::Or, m_line, m_col));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Bind, m_line, m_col));
				break;
			case '<':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::LessEqual, m_line, m_col));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Less, m_line, m_col));
				break;
			case '>':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::GreaterEqual, m_line, m_col));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Greater, m_line, m_col));
				break;
			case '?':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Question, m_line, m_col));
				break;
			case ':':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Colon, m_line, m_col));
				break;
			default:
				Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
				errorManager.UnexpectedCharacter(ch, m_line, m_col);
				break;
			}

			m_pos++;
			if (ch == '\n')
			{
				m_line++;
				m_col = 1;
			}
			else if (ch != '\r' && ch != '\t')
			{
				m_col++;
			}
		}

		m_tokens.push_back(std::make_shared<Token>(TokenType::_EOF, m_line, m_col));
	}

	void Lexer::Id()
	{
		std::string* id = new std::string();

		while (m_pos < m_code.size())
		{
			char ch = m_code[m_pos];
			if (isalpha(ch))
			{
				id->push_back(ch);
				m_pos++;
				m_col++;
			}
			else if (ch == '_')
			{
				id->push_back(ch);
				m_pos++;
				m_col++;
			}
			else if (isdigit(ch) && !id->empty())
			{
				id->push_back(ch);
				m_pos++;
				m_col++;
			}
			else if (id->empty()) {
				Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
				errorManager.UnexpectedEOF(m_line, m_col);
				break;
			}
			else {
				break;
			}
		}

		if (*id == "require")
			m_tokens.push_back(std::make_shared<Token>(TokenType::Require, m_line, m_col - id->size()));
		else if (*id == "if")
			m_tokens.push_back(std::make_shared<Token>(TokenType::If, m_line, m_col - id->size()));
		else if (*id == "else")
			m_tokens.push_back(std::make_shared<Token>(TokenType::Else, m_line, m_col - id->size()));
		else if (*id == "require")
			m_tokens.push_back(std::make_shared<Token>(TokenType::Require, m_line, m_col - id->size()));
		else
			m_tokens.push_back(std::make_shared<Token>(TokenType::Id, id, m_line, m_col - id->size()));
	}

	void Lexer::String()
	{
		std::string* id = new std::string();
		bool string_closed = false;

		// Skip opening string quote
		m_pos++;

		while (m_pos < m_code.size())
		{
			char ch = m_code[m_pos];
			if (ch != '"') {
				id->push_back(ch);
				m_pos++;
				m_col++;
			}
			else {
				m_pos++;
				m_col++;
				string_closed = true;
				break;
			}
		}

		m_tokens.push_back(std::make_shared<Token>(TokenType::String, id, m_line, m_col - id->size()));
	}

	void Lexer::Comment()
	{
		// we just skip comments
		while (m_pos < m_code.size())
		{
			char ch = m_code[m_pos];
			if (ch == '\n')
			{
				break;
			}
			m_pos++;
			m_col++;
		}
	}

	void Lexer::Number()
	{
		std::string* number = new std::string();
		bool already_met_point = false;

		while (m_pos < m_code.size())
		{
			char ch = m_code[m_pos];
			if (isdigit(ch))
			{
				number->push_back(ch);
				m_pos++;
				m_col++;
			}
			else if (ch == '.')
			{
				if (already_met_point) {
					Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
					errorManager.UnexpectedCharacter(ch, m_line, m_col);
					break;
				}
				else {
					already_met_point = true;
					number->push_back(ch);
					m_pos++;
					m_col++;
				}
			}
			else if (number->empty()) {
				Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
				errorManager.UnexpectedEOF(m_line, m_col);
				break;
			}
			else {
				break;
			}
		}

		if (already_met_point) {
			size_t pos = number->find('.');
			std::string digitstr = number->substr(pos + 1);
			if (digitstr.size() <= 7)
			{
				float* num = new float(std::stof(*number));
				m_tokens.push_back(std::make_shared<Token>(TokenType::Float, num, m_line, m_col - number->size()));
			}
			else {
				double* num = new double(std::stod(*number));
				m_tokens.push_back(std::make_shared<Token>(TokenType::Double, num, m_line, m_col - number->size()));
			}
		}
		else {
			int* num = new int(std::stoi(*number));
			m_tokens.push_back(std::make_shared<Token>(TokenType::Integer, num, m_line, m_col - number->size()));
		}
	}

	std::shared_ptr<Token> Lexer::GetToken()
	{
		if (m_index >= m_tokens.size())
		{
			return std::make_shared<Token>(TokenType::_EOF, 0, 0);
		}
		return m_tokens[m_index];
	}

	std::shared_ptr<Token> Lexer::SeekToken()
	{
		if (m_index + 1 >= m_tokens.size())
		{
			return std::make_shared<Token>(TokenType::_EOF, 0, 0);
		}
		return m_tokens[m_index + 1];
	}

	std::shared_ptr<Token> Lexer::NextToken()
	{
		if (m_index + 1 >= m_tokens.size())
		{
			return std::make_shared<Token>(TokenType::_EOF, 0, 0);
		}
		return m_tokens[m_index++];
	}
}
