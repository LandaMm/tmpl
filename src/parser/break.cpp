
#include "include/node/loop.h"
#include "include/parser.h"

namespace AST
{
    std::shared_ptr<Node> Parser::BreakStmt()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::Break);

        return std::make_shared<Nodes::BreakNode>(loc);
    }
}

