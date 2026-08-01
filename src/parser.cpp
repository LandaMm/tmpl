
#include "parser.h"
#include "error.h"
#include "token.h"
#include <memory>

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
				GetErrorManager().UnexpectedToken(m_lexer->GetFilename(), m_lexer->GetToken(), m_lexer->GetToken(), type);
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
	
	Node* Parser::CompileTimeStatement()
	{
		m_lexer->SaveState();
		
		Eat(TokenType::Id);

		auto next = m_lexer->SeekToken();

		m_lexer->RestoreState();

		switch (next->GetType())
		{
		case TokenType::OpenBracket:
			return FunctionDeclaration();
		default: // Type Declaration
			return TypeDeclaration();
		}

		assert(0 && "UNREACHABLE");
		return nullptr;
	}

	Node* Parser::Statement()
	{
		auto token = m_lexer->GetToken();
		Node* stmt = nullptr;
		switch (token->GetType())
		{
		case TokenType::If:
			stmt = IfElseStatement();
			break;
        case TokenType::Return:
            stmt = ReturnStatement();
            Eat(TokenType::Semicolon);
            break;
        case TokenType::While:
            stmt = WhileLoop();
            break;
        case TokenType::For:
            stmt = ForLoop();
            break;
        case TokenType::Break:
        {
            stmt = BreakStmt();
            Eat(TokenType::Semicolon);
            if (m_breaks.empty())
            {
                // FIXME:
                // Prelude::ErrorManager& manager = GetErrorManager();
                // manager.BreakNotAllowed(GetFilename(), std::dynamic_pointer_cast<Nodes::BreakNode>(stmt), "ParseError");
                return nullptr;
            }
            break;
        }
        case TokenType::Hash:
        {
            auto tokenType = Peek();
            if (tokenType == TokenType::_EOF)
            {
                Prelude::ErrorManager& manager = GetErrorManager();
                manager.UnexpectedEOF(GetFilename(), token->GetLine(), token->GetColumn());
                return nullptr;
            }
            Eat(TokenType::Hash);
            // Directive
            switch (tokenType)
            {
                case TokenType::Import:
                    stmt = ImportStatement();
                    break;
                case TokenType::Extern:
                    stmt = ExternStatement();
                    break;
                default:
					// TODO: better error
                    Prelude::ErrorManager& manager = GetErrorManager();
                    manager.UnexpectedToken(GetFilename(), m_lexer->SeekToken());
                    return nullptr;
            }
            break;
        }
		case TokenType::Id:
		{
			auto next = m_lexer->SeekToken();
			
			if (next->GetType() == TokenType::DoubleColon)
			{
				stmt = CompileTimeStatement();
			}
			else if (next->GetType() == TokenType::Colon) // Variable Declaration
			{
				stmt = VariableDeclaration();
			}
			else {
				stmt = Assignment();
			}

			Eat(TokenType::Semicolon);
			break;
		}
		default:
			auto& err = GetErrorManager();
			err.UnexpectedToken(m_lexer->GetFilename(), token);
			break;
		}
		return stmt;
	}
}
