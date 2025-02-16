#include "include/parser.h"

namespace AST
{
    std::shared_ptr<Nodes::TypeNode> Parser::Type()
    {
        auto target = Id();

        return std::make_shared<Nodes::TypeNode>(target, target->GetLocation());
    }
}

