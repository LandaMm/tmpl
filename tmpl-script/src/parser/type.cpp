#include "include/parser.h"

namespace AST
{
    std::shared_ptr<Nodes::TypeNode> Parser::Type()
    {
        auto target = Id();

        return std::make_shared<Nodes::TypeNode>(target, target->GetLocation());
    }

    std::shared_ptr<Nodes::TypeTemplateNode> Parser::TypeTemplate()
    {
        auto target = Id();

        return std::make_shared<Nodes::TypeTemplateNode>(target, target->GetLocation());
    }

    std::shared_ptr<Nodes::TypeDfNode> Parser::TypeDfStatement()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::TypeDf);

        auto tmpl = TypeTemplate();
        
        Eat(TokenType::Equal);

        auto value = Type();

        return std::make_shared<Nodes::TypeDfNode>(tmpl, value, loc);
    }
}

