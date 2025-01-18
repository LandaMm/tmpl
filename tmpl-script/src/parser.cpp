
#include"../include/parser.h"
#include"../include/error.h"

#include"../include/node/literal.h"
#include"../include/node/identifier.h"
#include"../include/node/function.h"
#include"../include/node/expression.h"

namespace Compiler
{
	void Parser::Error(std::string message)
	{
		Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
		errorManager.RaiseError("ParseError: " + message);
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
				Error("Unexpected end of code while requiring token of type " + std::to_string((int)type));
			}
			else
			{
				// TODO: provide better display of met token
				Error("Expected token of type " + std::to_string((int)type));
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
			auto closingToken = m_lexer->GetToken()->GetType();
			if (closingToken == TokenType::Newline)
			{
				Eat(TokenType::Newline);
			}
			else if (closingToken != TokenType::_EOF)
			{
				Error("Unexpected continious statement");
				break;
			}
		}
	}

	std::shared_ptr<Node> Parser::Statement()
	{
		auto token = m_lexer->GetToken();
		switch (token->GetType())
		{
		case TokenType::If:
			return IfElseStatement();
		default:
			return Ternary();
		}
	}
}
