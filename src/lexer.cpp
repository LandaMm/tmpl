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

			Location begin{ m_line, m_col };
			Location end{ m_line, m_col + 1 };

			HandleCharacter(ch);

			switch (ch)
			{
			case '\n':
				// skip newline
				// m_tokens.Push(m_arena.Alloc<Token>(TokenType::Newline));
				break;
			case ' ':
			case '\t':
			case '\r':
				// skip those characters
				// m_tokens.Push(m_arena.Alloc<Token>(TokenType::Whitespace));
				break;
			case '.':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::Point, begin, end));
				break;
			case ',':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::Comma, begin, end));
				break;
			case '(':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::OpenBracket, begin, end));
				break;
			case ')':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::CloseBracket, begin, end));
				break;
			case '{':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::OpenCurly, begin, end));
				break;
			case '}':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::CloseCurly, begin, end));
				break;
			case '[':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::OpenSquareBracket, begin, end));
				break;
			case ']':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::CloseSquareBracket, begin, end));
				break;
			case '+':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					end.col++;
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::CompoundAdd, begin, end));
					m_pos++;
					m_col++;
				}
				else
                    m_tokens.Push(m_arena.Alloc<Token>(TokenType::Plus, begin, end));
				break;
			case '@':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::At, begin, end));
				break;
			case '#':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::Hash, begin, end));
				break;
			case '-':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '>')
				{
					end.col++;
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::SingleArrow, begin, end));
					m_pos++;
					m_col++;
				}
				else if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					end.col++;
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::CompoundMinus, begin, end));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::Minus, begin, end));
				break;
			case '*':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					end.col++;
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::CompoundMultiply, begin, end));
					m_pos++;
					m_col++;
				}
				else
                    m_tokens.Push(m_arena.Alloc<Token>(TokenType::Multiply, begin, end));
				break;
			case '/':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					end.col++;
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::CompoundDivide, begin, end));
					m_pos++;
					m_col++;
				}
				else
                    m_tokens.Push(m_arena.Alloc<Token>(TokenType::Divide, begin, end));
				break;
			case ';':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::Semicolon, begin, end));
				break;
			case '=':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					end.col++;
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::Compare, begin, end));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::Equal, begin, end));
				break;
			case '!':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					end.col++;
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::NotEqual, begin, end));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::Not, begin, end));
				break;
			case '&':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '&')
				{
					end.col++;
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::And, begin, end));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::Ampersand, begin, end));
				break;
			case '|':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '|')
				{
					end.col++;
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::Or, begin, end));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::Bind, begin, end));
				break;
			case '<':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					end.col++;
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::LessEqual, begin, end));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::Less, begin, end));
				break;
			case '>':
				if (m_pos < m_code.Size() && m_code[m_pos + 1] == '=')
				{
					end.col++;
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::GreaterEqual, begin, end));
					m_pos++;
					m_col++;
				}
				else
					m_tokens.Push(m_arena.Alloc<Token>(TokenType::Greater, begin, end));
				break;
			case '?':
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::Question, begin, end));
				break;
			case ':':
				if (m_pos < m_code.Size())
				{
					if (m_code[m_pos + 1] == ':')
					{
						end.col++;
						m_tokens.Push(m_arena.Alloc<Token>(TokenType::DoubleColon, begin, end));
						m_pos++;
						m_col++;
						break;
					}
					else if (m_code[m_pos + 1] == '=')
					{
						end.col++;
						m_tokens.Push(m_arena.Alloc<Token>(TokenType::ColonEqual, begin, end));
						m_pos++;
						m_col++;
						break;
					}
				}
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::Colon, begin, end));
				break;
			default:
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.UnexpectedCharacter(GetFilename(), ch, m_line, m_col);
				break;
			}

			m_pos++;
		}

		Location begin{ m_line, m_col };
		m_tokens.Push(m_arena.Alloc<Token>(TokenType::_EOF, begin, begin));
	}

	void Lexer::Id()
	{
		String *id = new String();

		Location begin{ m_line, m_col };

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

		Location end{ m_line, m_col };

		if (*id == "import")
			m_tokens.Push(m_arena.Alloc<Token>(TokenType::Import, begin, end));
		else if (*id == "if")
			m_tokens.Push(m_arena.Alloc<Token>(TokenType::If, begin, end));
		else if (*id == "else")
			m_tokens.Push(m_arena.Alloc<Token>(TokenType::Else, begin, end));
		else if (*id == "extern")
			m_tokens.Push(m_arena.Alloc<Token>(TokenType::Extern, begin, end));
		else if (*id == "var")
			m_tokens.Push(m_arena.Alloc<Token>(TokenType::Var, begin, end));
		else if (*id == "const")
			m_tokens.Push(m_arena.Alloc<Token>(TokenType::Const, begin, end));
		else if (*id == "return")
			m_tokens.Push(m_arena.Alloc<Token>(TokenType::Return, begin, end));
		else if (*id == "true")
            m_tokens.Push(m_arena.Alloc<Token>(TokenType::True, begin, end));
		else if (*id == "false")
            m_tokens.Push(m_arena.Alloc<Token>(TokenType::False, begin, end));
		else if (*id == "typedf")
            m_tokens.Push(m_arena.Alloc<Token>(TokenType::TypeDf, begin, end));
		else if (*id == "new")
            m_tokens.Push(m_arena.Alloc<Token>(TokenType::New, begin, end));
		else if (*id == "while")
            m_tokens.Push(m_arena.Alloc<Token>(TokenType::While, begin, end));
		else if (*id == "for")
            m_tokens.Push(m_arena.Alloc<Token>(TokenType::For, begin, end));
		else if (*id == "break")
            m_tokens.Push(m_arena.Alloc<Token>(TokenType::Break, begin, end));
		else
		{
			Token::TypedValueHolder<String>* value = m_arena.Alloc<Token::TypedValueHolder<String>>(m_arena.Alloc<String>(*id));
			m_tokens.Push(m_arena.Alloc<Token>(TokenType::Id, value, begin, end));
		}
	}

	void Lexer::StringLiteral()
	{
		String *id = new String();
		bool string_closed = false;

		Location begin(m_line, m_col);

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

		Token::TypedValueHolder<String>* value = m_arena.Alloc<Token::TypedValueHolder<String>>(m_arena.Alloc<String>(*id));

		Location end(m_line, m_col + 1);

		m_tokens.Push(m_arena.Alloc<Token>(TokenType::String, value, begin, end)); // additional column for closing quote
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

		Location begin(m_line, m_col);

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

		Location end(m_line, m_col);

		if (already_met_point)
		{
			size_t pos = number->Find('.');
			String digitstr = number->Substr(pos + 1);
			if (digitstr.Size() <= 7)
			{
				Token::TypedValueHolder<float>* value = m_arena.Alloc<Token::TypedValueHolder<float>>(m_arena.Alloc<float>(std::stof(number->c_str())));
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::Float, value, begin, end));
			}
			else
			{
				Token::TypedValueHolder<double>* value = m_arena.Alloc<Token::TypedValueHolder<double>>(m_arena.Alloc<double>(std::stod(number->c_str())));
				m_tokens.Push(m_arena.Alloc<Token>(TokenType::Double, value, begin, end));
			}
		}
		else
		{
			Token::TypedValueHolder<int>* value = m_arena.Alloc<Token::TypedValueHolder<int>>(m_arena.Alloc<int>(std::stoi(number->c_str())));
			m_tokens.Push(m_arena.Alloc<Token>(TokenType::Integer, value, begin, end));
		}
	}

	Token* Lexer::GetToken()
	{
		if (m_index >= m_tokens.Size())
		{
			Location loc(m_line, m_col);
			return m_arena.Alloc<Token>(TokenType::_EOF, loc, loc);
		}
		return m_tokens[m_index];
	}

    void Lexer::RestoreState()
    {
        assert(m_state != nullptr && "State is required for restoring.");

        m_index = m_state->index;

        m_state = nullptr;
    }

	Token* Lexer::SeekToken()
	{
		if (m_index + 1 >= m_tokens.Size())
		{
			Location loc(m_line, m_col);
			return m_arena.Alloc<Token>(TokenType::_EOF, loc, loc);
		}
		return m_tokens[m_index + 1];
	}

	Token* Lexer::NextToken()
	{
		if (m_index + 1 >= m_tokens.Size())
		{
			Location loc(m_line, m_col);
			return m_arena.Alloc<Token>(TokenType::_EOF, loc, loc);
		}
		return m_tokens[m_index++];
	}

	Token* Lexer::PrevToken()
	{
		if (m_index > 0)
		{
			return m_tokens[m_index - 1];
		}
		return nullptr;
	}
}
