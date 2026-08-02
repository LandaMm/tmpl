
#include "parser.h"

namespace AST
{
	Node* Parser::IfElseStatement()
	{
		// if 5 == 5 ? true : false {} else {}
        auto loc = m_lexer->GetToken()->GetLocation();
		Eat(TokenType::If);
		// Eat(TokenType::OpenBracket);
		Node* condition = Ternary();
		// Eat(TokenType::CloseBracket);

		auto ifElse = m_arena.Alloc<Statements::IfElseStatement>(condition, loc);
        Statements::StatementsBody* body =
            m_arena.Alloc<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());

		if (m_lexer->GetToken()->GetType() != TokenType::OpenCurly)
		{
			body->AddItem(Statement());
		}
		else
		{
			Eat(TokenType::OpenCurly);
			while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
			{
				Node* statement = Statement();
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
				Node* elseNode = IfElseStatement();
				ifElse->SetElseStatement(elseNode);
			}
			else
			{
				auto stmts = m_arena.Alloc<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());
				if (m_lexer->GetToken()->GetType() != TokenType::OpenCurly)
				{
					stmts->AddItem(Statement());
				}
				else
				{
					Eat(TokenType::OpenCurly);
					while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
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

