
#include "node/loop.hpp"
#include "parser.h"

namespace AST
{
    Node* Parser::BreakStmt()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::Break);

        return m_arena.Alloc<Nodes::BreakNode>(loc);
    }
}

