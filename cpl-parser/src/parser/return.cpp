
#include "cpl-parser/parser.h"
#include "cpl-parser/node/return.hpp"

namespace AST
{
    Node* Parser::ReturnStatement()
    {
        auto token = m_lexer->GetToken();
        Eat(TokenType::Return);

        Node* value = Ternary();

        return m_arena.Alloc<Nodes::ReturnNode>(value, token->GetLocation());
    }
}
