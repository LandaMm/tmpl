

#include "../../include/parser.h"
#include "../../include/node/generic.h"
#include <error.h>
#include <memory>

namespace AST
{
    std::shared_ptr<Node> Parser::GenericType(std::shared_ptr<Nodes::IdentifierNode> target)
    {
        Eat(TokenType::Less);

        if (m_lexer->GetToken()->GetType() != TokenType::Id)
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.UnexpectedToken(GetFilename(), m_lexer->GetToken(), m_lexer->GetToken(), TokenType::Id);
            return nullptr;
        }

        auto id = Id();

        if (m_lexer->GetToken()->GetType() == TokenType::Less)
        {
            auto nextGeneric = GenericType(id);
            return std::make_shared<Nodes::GenericNode>(target, std::dynamic_pointer_cast<Node>(nextGeneric), target->GetLocation());
        }

        return std::make_shared<Nodes::GenericNode>(target, id, target->GetLocation());
    }
}

