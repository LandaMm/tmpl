
#include "cpl-parser/node/loop.hpp"
#include "cpl-parser/parser.h"

namespace AST
{
    Node* Parser::BreakStmt()
    {
        auto loc = Current()->GetLocation();
        Eat(TokenType::Break);

        return m_arena.Alloc<Nodes::BreakNode>(loc);
    }
}

