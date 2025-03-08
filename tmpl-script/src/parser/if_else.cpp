
#include "include/parser.h"

namespace AST
{
	std::shared_ptr<Node> Parser::IfElseStatement()
	{
		// if 5 == 5 ? true : false {} else {}
        auto loc = m_lexer->GetToken()->GetLocation();
		Eat(TokenType::If);
		// Eat(TokenType::OpenBracket);
		std::shared_ptr<Node> condition = Ternary();
		// Eat(TokenType::CloseBracket);

		auto ifElse = std::make_shared<Statements::IfElseStatement>(condition, loc);
        std::shared_ptr<Statements::StatementsBody> body =
            std::make_shared<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());

		if (m_lexer->GetToken()->GetType() != TokenType::OpenCurly)
		{
			body->AddItem(Statement());
		}
		else
		{
			Eat(TokenType::OpenCurly);
			while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
			{
				std::shared_ptr<Node> statement = Statement();
				body->AddItem(statement);
			}
			Eat(TokenType::CloseCurly);
		}

        ifElse->SetBody(body);

		if (m_lexer->GetToken()->GetType() == TokenType::Else)
		{
			Eat(TokenType::Else);

			if (m_lexer->GetToken()->GetType() == TokenType::If)
			{
				std::shared_ptr<Node> elseNode = IfElseStatement();
				ifElse->SetElseStatement(elseNode);
			}
			else
			{
				auto stmts = std::make_shared<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());
				if (m_lexer->GetToken()->GetType() != TokenType::OpenCurly)
				{
					stmts->AddItem(Statement());
				}
				else
				{
					Eat(TokenType::OpenCurly);
					while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
					{
						std::shared_ptr<Node> statement = Statement();
						stmts->AddItem(statement);
					}
					Eat(TokenType::CloseCurly);
				}
				ifElse->SetElseStatement(stmts);
			}
		}

		return ifElse;
	}
}

