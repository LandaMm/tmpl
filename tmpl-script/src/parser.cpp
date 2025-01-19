
#include"../include/parser.h"
#include"../include/error.h"

#include"../include/node/literal.h"
#include"../include/node/identifier.h"
#include"../include/node/function.h"
#include"../include/node/expression.h"

namespace Compiler
{
	Prelude::ErrorManager& Parser::GetErrorManager()
	{
		Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
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
				// TODO: provide better display of met token
				GetErrorManager().UnexpectedToken(m_lexer->GetToken(), type);
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
		default:
			stmt = Ternary();
			Eat(TokenType::Semicolon);
			break;
		}
		return stmt;
	}
}
