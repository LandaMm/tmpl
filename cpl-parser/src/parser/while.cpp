
#include "cpl-parser/parser.h"
#include "cpl-parser/node/loop.hpp"

namespace AST
{
    Node* Parser::WhileLoop()
    {
        auto loc = Current()->GetLocation();
        Eat(TokenType::While);

        Eat(TokenType::OpenBracket);
        auto condition = Ternary();
        Eat(TokenType::CloseBracket);

        Statements::StatementsBody* body =
            m_arena.Alloc<Statements::StatementsBody>(Current()->GetLocation());

        m_breaks.push_back(loc);

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

        m_breaks.pop_back();

        return m_arena.Alloc<Nodes::WhileNode>(condition, body, loc);
    }
}

