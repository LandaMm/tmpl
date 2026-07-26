#include <memory>
#include <cassert>

#include "basics/file.hpp"
#include "error.h"
#include "lexer.h"

namespace AST
{
	Lexer::Lexer(FileReader& reader, String filename)
	{
		m_filename = filename;

		m_index = 0;
		m_pos = 0;
		m_line = 1;
		m_col = 0;

		m_code = reader.ReadEntireText();
	}

	void Lexer::HandleCharacter(char ch)
	{
		if (ch == '\n')
		{
			m_line++;
			m_col = 0;
		}
		else if (ch != '\r')
		{
			m_col++;
		}
	}

	void Lexer::Tokenize()
	{
		while (m_pos < m_code.Size())
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
				StringLiteral();
				continue;
			}

			if (ch == '/' && m_pos + 1 < m_code.Size() && m_code[m_pos + 1] == '/')
			{
				// skip comment symbol
				m_pos += 2;
				Comment();
				continue;
			}

			HandleCharacter(ch);

			switch (ch)
			{
			case '\n':
				// skip newline
				// m_tokens.Push(std::make_shared<Token>(TokenType::Newline));
				break;
			case ' ':
			case '\t':
			case '\r':
				// skip those characters
				// m_tokens.Push(std::make_shared<Token>(TokenType::Whitespace));
				break;
			case '.':
				m_tokens.Push(std::make_shared<Token>(TokenType::Point, m_line, m_col));
				break;
			case ',':
				m_tokens.Push(std::make_shared<Token>(TokenType::Comma, m_line, m_col));
				break;
			case '(':
				m_tokens.Push(std::make_shared<Token>(TokenType::OpenBracket, m_line, m_col));
				break;
			case ')':
				m_tokens.Push(std::make_shared<Token>(TokenType::CloseBracket, m_line, m_col));
				break;
			case '{':
				m_tokens.Push(std::make_shared<Token>(TokenType::OpenCurly, m_line, m_col));
				break;
			case '}':
				m_tokens.Push(std::make_shared<Token>(TokenType::CloseCurly, m_line, m_col));
				break;
			case '[':
				m_tokens.Push(std::make_shared<Token>(TokenType::OpenSquareBracket, m_line, m_col));
				break;
			case ']':
				m_tokens.Push(std::make_shared<Token>(TokenType::CloseSquareBracket, m_line, m_col));
				break;
			case '+':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.Push(std::make_shared<Token>(TokenType::CompoundAdd, m_line, m_col));
					m_pos++;
					m_col++;
				}
				else
                    m_tokens.Push(std::make_shared<Token>(TokenType::Plus, m_line, m_col));
				break;
			case '@':
				m_tokens.Push(std::make_shared<Token>(TokenType::At, m_line, m_col));
				break;
			case '#':
				m_tokens.Push(std::make_shared<Token>(TokenType::Hash, m_line, m_col));
				break;
			case '-':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '>')
				{
					m_tokens.Push(std::make_shared<Token>(TokenType::SingleArrow, m_line, m_col));
					m_pos++;
					m_col++;
				}
				else if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.Push(std::make_shared<Token>(TokenType::CompoundMinus, m_line, m_col));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(std::make_shared<Token>(TokenType::Minus, m_line, m_col));
				break;
			case '*':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.Push(std::make_shared<Token>(TokenType::CompoundMultiply, m_line, m_col));
					m_pos++;
					m_col++;
				}
				else
                    m_tokens.Push(std::make_shared<Token>(TokenType::Multiply, m_line, m_col));
				break;
			case '/':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.Push(std::make_shared<Token>(TokenType::CompoundDivide, m_line, m_col));
					m_pos++;
					m_col++;
				}
				else
                    m_tokens.Push(std::make_shared<Token>(TokenType::Divide, m_line, m_col));
				break;
			case ';':
				m_tokens.Push(std::make_shared<Token>(TokenType::Semicolon, m_line, m_col));
				break;
			case '=':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.Push(std::make_shared<Token>(TokenType::Compare, m_line, m_col));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(std::make_shared<Token>(TokenType::Equal, m_line, m_col));
				break;
			case '!':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.Push(std::make_shared<Token>(TokenType::NotEqual, m_line, m_col));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(std::make_shared<Token>(TokenType::Not, m_line, m_col));
				break;
			case '&':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '&')
				{
					m_tokens.Push(std::make_shared<Token>(TokenType::And, m_line, m_col));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(std::make_shared<Token>(TokenType::Ampersand, m_line, m_col));
				break;
			case '|':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '|')
				{
					m_tokens.Push(std::make_shared<Token>(TokenType::Or, m_line, m_col));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(std::make_shared<Token>(TokenType::Bind, m_line, m_col));
				break;
			case '<':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.Push(std::make_shared<Token>(TokenType::LessEqual, m_line, m_col));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(std::make_shared<Token>(TokenType::Less, m_line, m_col));
				break;
			case '>':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					m_tokens.Push(std::make_shared<Token>(TokenType::GreaterEqual, m_line, m_col));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(std::make_shared<Token>(TokenType::Greater, m_line, m_col));
				break;
			case '?':
				m_tokens.Push(std::make_shared<Token>(TokenType::Question, m_line, m_col));
				break;
			case ':':
				if (m_pos < m_code.Size())
				{
					if (m_code[m_pos + 1] == ':')
					{
						m_tokens.Push(std::make_shared<Token>(TokenType::DoubleColon, m_line, m_col));
						m_pos++;
						m_col++;
						break;
					}
					else if (m_code[m_pos + 1] == '=')
					{
						m_tokens.Push(std::make_shared<Token>(TokenType::ColonEqual, m_line, m_col));
						m_pos++;
						m_col++;
						break;
					}
				}
				m_tokens.Push(std::make_shared<Token>(TokenType::Colon, m_line, m_col));
				break;
			default:
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.UnexpectedCharacter(GetFilename(), ch, m_line, m_col);
				break;
			}

			m_pos++;
		}

		m_tokens.Push(std::make_shared<Token>(TokenType::_EOF, m_line, m_col));
	}

	void Lexer::Id()
	{
		String *id = new String();

		while (m_pos < m_code.Size())
		{
			char ch = m_code[m_pos];
			if (isalpha(ch))
			{
				id->Extend(ch);
				m_pos++;
				m_col++;
			}
			else if (ch == '_')
			{
				id->Extend(ch);
				m_pos++;
				m_col++;
			}
			else if (isdigit(ch) && !id->Empty())
			{
				id->Extend(ch);
				m_pos++;
				m_col++;
			}
			else if (id->Empty())
			{
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.UnexpectedEOF(GetFilename(), m_line, m_col);
				break;
			}
			else
			{
				break;
			}
		}

		if (*id == "import")
			m_tokens.Push(std::make_shared<Token>(TokenType::Import, m_line, m_col));
		else if (*id == "if")
			m_tokens.Push(std::make_shared<Token>(TokenType::If, m_line, m_col));
		else if (*id == "else")
			m_tokens.Push(std::make_shared<Token>(TokenType::Else, m_line, m_col));
		else if (*id == "extern")
			m_tokens.Push(std::make_shared<Token>(TokenType::Extern, m_line, m_col));
		else if (*id == "var")
			m_tokens.Push(std::make_shared<Token>(TokenType::Var, m_line, m_col));
		else if (*id == "const")
			m_tokens.Push(std::make_shared<Token>(TokenType::Const, m_line, m_col));
		else if (*id == "return")
			m_tokens.Push(std::make_shared<Token>(TokenType::Return, m_line, m_col));
		else if (*id == "true")
            m_tokens.Push(std::make_shared<Token>(TokenType::True, m_line, m_col));
		else if (*id == "false")
            m_tokens.Push(std::make_shared<Token>(TokenType::False, m_line, m_col));
		else if (*id == "typedf")
            m_tokens.Push(std::make_shared<Token>(TokenType::TypeDf, m_line, m_col));
		else if (*id == "new")
            m_tokens.Push(std::make_shared<Token>(TokenType::New, m_line, m_col));
		else if (*id == "while")
            m_tokens.Push(std::make_shared<Token>(TokenType::While, m_line, m_col));
		else if (*id == "for")
            m_tokens.Push(std::make_shared<Token>(TokenType::For, m_line, m_col));
		else if (*id == "break")
            m_tokens.Push(std::make_shared<Token>(TokenType::Break, m_line, m_col));
		else
		{
			std::shared_ptr<Token::TypedValueHolder<String>> value = std::make_shared<Token::TypedValueHolder<String>>(std::make_shared<String>(*id));
			m_tokens.Push(std::make_shared<Token>(TokenType::Id, value, m_line, m_col));
		}
	}

	void Lexer::StringLiteral()
	{
		String *id = new String();
		bool string_closed = false;

		// Skip opening string quote
		m_pos++;

		while (m_pos < m_code.Size())
		{
			char ch = m_code[m_pos];
            if (ch == '\\')
            {
                m_pos++;
                m_col++;
                if (m_pos >= m_code.Size())
                {
                    string_closed = false;
                    break;
                }
                ch = m_code[m_pos];
                switch (ch)
                {
                    case '\\':
                        id->Extend('\\');
                        break;
                    case 'n':
                        id->Extend('\n');
                        break;
                    case 'r':
                        id->Extend('\r');
                        break;
                    case 't':
                        id->Extend('\t');
                        break;
                    case 'b':
                        id->Extend('\b');
                        break;
                    case '"':
                        id->Extend('"');
                        break;
                    default:
                    {
                        Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
                        errManager.UnexpectedEscapeCharacter(GetFilename(), ch, m_line, m_col);
                        return;
                    }
                }
                m_pos++;
                m_col++;
            }
            else if (ch != '"')
			{
				id->Extend(ch);
				m_pos++;
				m_col++;
			}
            else
			{
				m_pos++;
				m_col++;
				string_closed = true;
				break;
			}
		}

		std::shared_ptr<Token::TypedValueHolder<String>> value = std::make_shared<Token::TypedValueHolder<String>>(std::make_shared<String>(*id));
		m_tokens.Push(std::make_shared<Token>(TokenType::String, value, m_line, m_col + 1)); // additional column for closing quote
	}

	void Lexer::Comment()
	{
		// we just skip comments
		while (m_pos < m_code.Size())
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
		String *number = new String();
		bool already_met_point = false;

		while (m_pos < m_code.Size())
		{
			char ch = m_code[m_pos];
			if (isdigit(ch))
			{
				number->Extend(ch);
				m_pos++;
				m_col++;
			}
			else if (ch == '.')
			{
				if (already_met_point)
				{
					Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
					errorManager.UnexpectedCharacter(GetFilename(), ch, m_line, m_col);
					break;
				}
				else
				{
					already_met_point = true;
					number->Extend(ch);
					m_pos++;
					m_col++;
				}
			}
			else if (number->Empty())
			{
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.UnexpectedEOF(GetFilename(), m_line, m_col);
				break;
			}
			else
			{
				break;
			}
		}

		if (already_met_point)
		{
			size_t pos = number->Find('.');
			String digitstr = number->Substr(pos + 1);
			if (digitstr.Size() <= 7)
			{
				std::shared_ptr<Token::TypedValueHolder<float>> value = std::make_shared<Token::TypedValueHolder<float>>(std::make_shared<float>(std::stof(number->c_str())));
				m_tokens.Push(std::make_shared<Token>(TokenType::Float, value, m_line, m_col));
			}
			else
			{
				std::shared_ptr<Token::TypedValueHolder<double>> value = std::make_shared<Token::TypedValueHolder<double>>(std::make_shared<double>(std::stod(number->c_str())));
				m_tokens.Push(std::make_shared<Token>(TokenType::Double, value, m_line, m_col));
			}
		}
		else
		{
			std::shared_ptr<Token::TypedValueHolder<int>> value = std::make_shared<Token::TypedValueHolder<int>>(std::make_shared<int>(std::stoi(number->c_str())));
			m_tokens.Push(std::make_shared<Token>(TokenType::Integer, value, m_line, m_col));
		}
	}

	std::shared_ptr<Token> Lexer::GetToken()
	{
		if (m_index >= m_tokens.Size())
		{
			return std::make_shared<Token>(TokenType::_EOF, 0, 0);
		}
		return m_tokens[m_index];
	}

    void Lexer::RestoreState()
    {
        assert(m_state != nullptr && "State is required for restoring.");

        m_index = m_state->index;

        m_state = nullptr;
    }

	std::shared_ptr<Token> Lexer::SeekToken()
	{
		if (m_index + 1 >= m_tokens.Size())
		{
			return std::make_shared<Token>(TokenType::_EOF, 0, 0);
		}
		return m_tokens[m_index + 1];
	}

	std::shared_ptr<Token> Lexer::NextToken()
	{
		if (m_index + 1 >= m_tokens.Size())
		{
			return std::make_shared<Token>(TokenType::_EOF, 0, 0);
		}
		return m_tokens[m_index++];
	}

	std::shared_ptr<Token> Lexer::PrevToken()
	{
		if (m_index > 0)
		{
			return m_tokens[m_index - 1];
		}
		return nullptr;
	}
}
