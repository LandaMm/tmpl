
#include "include/parser.h"
#include "include/node/return.h"

namespace AST
{
    std::shared_ptr<Node> Parser::ReturnStatement()
    {
        auto token = m_lexer->GetToken();
        Eat(TokenType::Return);

        std::shared_ptr<Node> value = Ternary();

        return std::make_shared<Nodes::ReturnNode>(value, token->GetLocation());
    }
}
