
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
				GetErrorManager().UnexpectedEofWhileToken(GetFilename(), type, token->GetLine(), token->GetColumn());
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
        case TokenType::Export:
            stmt = ExportStmt();
            break;
        case TokenType::At:
        {
            auto tokenType = Peek();
            if (tokenType == TokenType::_EOF)
            {
                Prelude::ErrorManager& manager = GetErrorManager();
                manager.UnexpectedEOF(GetFilename(), token->GetLine(), token->GetColumn());
                return nullptr;
            }
            Eat(TokenType::At);
            // Macros
            switch (tokenType)
            {
                case TokenType::Require:
                    stmt = RequireStatement();
                    break;
                case TokenType::Extern:
                    stmt = ExternStatement();
                    break;
                default:
                    Prelude::ErrorManager& manager = GetErrorManager();
                    manager.UnexpectedToken(GetFilename(), m_lexer->SeekToken());
                    return nullptr;
            }
            break;
        }
		default:
			stmt = Ternary();
			Eat(TokenType::Semicolon);
			break;
		}
		return stmt;
	}
}
