
#include"../include/error.h"
#include"../include/lexer.h"

namespace Compiler
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
				m_tokens.push_back(std::make_shared<Token>(TokenType::Newline));
				break;
			case ' ':
			case '\r':
				// skip those characters
				//m_tokens.push_back(std::make_shared<Token>(TokenType::Whitespace));
				break;
			case '.':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Point));
				break;
			case ',':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Comma));
				break;
			case '(':
				m_tokens.push_back(std::make_shared<Token>(TokenType::OpenBracket));
				break;
			case ')':
				m_tokens.push_back(std::make_shared<Token>(TokenType::CloseBracket));
				break;
			case '{':
				m_tokens.push_back(std::make_shared<Token>(TokenType::OpenCurly));
				break;
			case '}':
				m_tokens.push_back(std::make_shared<Token>(TokenType::CloseCurly));
				break;
			case '[':
				m_tokens.push_back(std::make_shared<Token>(TokenType::OpenSquareBracket));
				break;
			case ']':
				m_tokens.push_back(std::make_shared<Token>(TokenType::CloseSquareBracket));
				break;
			case '+':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Plus));
				break;
			case '-':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Minus));
				break;
			case '*':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Multiply));
				break;
			case '/':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Divide));
				break;
			case '=':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::Compare));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Equal));
				break;
			case '!':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::NotEqual));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Not));
				break;
			case '&':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '&')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::And));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Ampersand));
				break;
			case '|':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '|')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::Or));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Bind));
				break;
			case '<':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::LessEqual));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Less));
				break;
			case '>':
				if (m_pos < m_code.size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.push_back(std::make_shared<Token>(TokenType::GreaterEqual));
					m_pos++;
				}
				else
					m_tokens.push_back(std::make_shared<Token>(TokenType::Greater));
				break;
			case '?':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Question));
				break;
			case ':':
				m_tokens.push_back(std::make_shared<Token>(TokenType::Colon));
				break;
			default:
				Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
				errorManager.RaiseError("LexerError: Unexpected character met: '" + std::to_string(ch) + "'");
				break;
			}

			m_pos++;
		}

		m_tokens.push_back(std::make_shared<Token>(TokenType::_EOF));
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
			}
			else if (ch == '_')
			{
				id->push_back(ch);
				m_pos++;
			}
			else if (isdigit(ch) && !id->empty())
			{
				id->push_back(ch);
				m_pos++;
			}
			else if (id->empty()) {
				Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
				errorManager.RaiseError("LexerError: Unexpected end of code while parsing id");
				break;
			}
			else {
				break;
			}
		}

		if (*id == "require")
			m_tokens.push_back(std::make_shared<Token>(TokenType::Require));
		else if (*id == "if")
			m_tokens.push_back(std::make_shared<Token>(TokenType::If));
		else if (*id == "else")
			m_tokens.push_back(std::make_shared<Token>(TokenType::Else));
		else if (*id == "require")
			m_tokens.push_back(std::make_shared<Token>(TokenType::Require));
		else
			m_tokens.push_back(std::make_shared<Token>(TokenType::Id, id));
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
			}
			else {
				m_pos++;
				string_closed = true;
				break;
			}
		}

		m_tokens.push_back(std::make_shared<Token>(TokenType::String, id));
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
			}
			else if (ch == '.')
			{
				if (already_met_point) {
					Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
					errorManager.RaiseError("LexerError: Unexpected additional point while parsing decimal");
					break;
				}
				else {
					already_met_point = true;
					number->push_back(ch);
					m_pos++;
				}
			}
			else if (number->empty()) {
				Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
				errorManager.RaiseError("LexerError: Unexpected end of code while parsing id");
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
				m_tokens.push_back(std::make_shared<Token>(TokenType::Float, num));
			}
			else {
				double* num = new double(std::stod(*number));
				m_tokens.push_back(std::make_shared<Token>(TokenType::Double, num));
			}
		}
		else {
			int* num = new int(std::stoi(*number));
			m_tokens.push_back(std::make_shared<Token>(TokenType::Integer, num));
		}
	}

	std::shared_ptr<Token> Lexer::GetToken()
	{
		if (m_index >= m_tokens.size())
		{
			return std::make_shared<Token>(TokenType::_EOF);
		}
		return m_tokens[m_index];
	}

	std::shared_ptr<Token> Lexer::SeekToken()
	{
		if (m_index + 1 >= m_tokens.size())
		{
			return std::make_shared<Token>(TokenType::_EOF);
		}
		return m_tokens[m_index + 1];
	}

	std::shared_ptr<Token> Lexer::NextToken()
	{
		if (m_index + 1 >= m_tokens.size())
		{
			return std::make_shared<Token>(TokenType::_EOF);
		}
		return m_tokens[m_index++];
	}
}
