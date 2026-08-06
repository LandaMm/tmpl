
#include "cpl-parser/parser.h"

namespace AST
{
	Node* Parser::IfElseStatement()
	{
		// if 5 == 5 ? true : false {} else {}
        auto loc = Current()->GetLocation();
		Eat(TokenType::If);
		// Eat(TokenType::OpenBracket);
		Node* condition = Ternary();
		// Eat(TokenType::CloseBracket);

		auto ifElse = m_arena.Alloc<Statements::IfElseStatement>(condition, loc);
        Statements::StatementsBody* body =
            m_arena.Alloc<Statements::StatementsBody>(Current()->GetLocation());

		if (Current()->GetType() != TokenType::OpenCurly)
		{
			body->AddItem(Statement());
		}
		else
		{
			Eat(TokenType::OpenCurly);
			while (Current()->GetType() != TokenType::CloseCurly && Current()->GetType() != TokenType::_EOF)
			{
				Node* statement = Statement();
				body->AddItem(statement);
			}
			Eat(TokenType::CloseCurly);
		}

        ifElse->SetBody(body);

		if (Current()->GetType() == TokenType::Else)
		{
			Eat(TokenType::Else);

			if (Current()->GetType() == TokenType::If)
			{
				Node* elseNode = IfElseStatement();
				ifElse->SetElseStatement(elseNode);
			}
			else
			{
				auto stmts = m_arena.Alloc<Statements::StatementsBody>(Current()->GetLocation());
				if (Current()->GetType() != TokenType::OpenCurly)
				{
					stmts->AddItem(Statement());
				}
				else
				{
					Eat(TokenType::OpenCurly);
					while (Current()->GetType() != TokenType::CloseCurly && Current()->GetType() != TokenType::_EOF)
					{
						Node* statement = Statement();
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

