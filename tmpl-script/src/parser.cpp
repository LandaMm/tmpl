
#include "../include/parser.h"
#include "../include/error.h"

namespace AST
{
	Prelude::ErrorManager &Parser::GetErrorManager()
	{
		Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
		return errorManager;
	}

	void Parser::Eat(TokenType type)
	{
		if (m_lexer->GetToken()->GetType() == type)
		{
			m_lexer->NextToken();
		}
		else
		{
			if (m_lexer->GetToken()->GetType() == TokenType::_EOF)
			{
				auto token = m_lexer->GetToken();
				GetErrorManager().UnexpectedEofWhileToken(type, token->GetLine(), token->GetColumn());
			}
			else
			{
				if (std::shared_ptr<Token> prev = m_lexer->PrevToken())
				{
					GetErrorManager().UnexpectedToken(m_lexer->GetFilename(), prev, m_lexer->GetToken(), type);
				}
				else
				{
					GetErrorManager().UnexpectedToken(m_lexer->GetFilename(), m_lexer->GetToken(), m_lexer->GetToken(), type);
				}
			}
		}
	}

	TokenType Parser::Peek()
	{
		return m_lexer->SeekToken()->GetType();
	}

	void Parser::Parse()
	{
		while (m_lexer->GetToken()->GetType() != TokenType::_EOF)
		{
			m_root->AddStatement(Statement());
		}
	}

	std::shared_ptr<Node> Parser::Statement()
	{
		auto token = m_lexer->GetToken();
		std::shared_ptr<Node> stmt = nullptr;
		switch (token->GetType())
		{
		case TokenType::If:
			stmt = IfElseStatement();
			break;
		case TokenType::Var:
		case TokenType::Const:
			stmt = VariableDeclaration();
			Eat(TokenType::Semicolon);
			break;
		case TokenType::SingleArrow:
			stmt = ProcedureDeclaration();
			break;
        case TokenType::Return:
            stmt = ReturnStatement();
            Eat(TokenType::Semicolon);
            break;
        case TokenType::Fn:
            stmt = FunctionDeclaration();
            break;
		default:
			stmt = Ternary();
			Eat(TokenType::Semicolon);
			break;
		}
		return stmt;
	}
}
