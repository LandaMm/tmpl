
#include "parser.h"
#include "node/loop.hpp"

namespace AST
{
    Node* Parser::WhileLoop()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::While);

        Eat(TokenType::OpenBracket);
        auto condition = Ternary();
        Eat(TokenType::CloseBracket);

        Statements::StatementsBody* body =
            m_arena.Alloc<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());

        m_breaks.push_back(loc);

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

        m_breaks.pop_back();

        return m_arena.Alloc<Nodes::WhileNode>(condition, body, loc);
    }
}

