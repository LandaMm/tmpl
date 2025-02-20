#include "include/parser.h"
#include "include/token.h"

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

    std::shared_ptr<Nodes::CastNode> Parser::Cast(std::shared_ptr<Nodes::TypeNode> typ)
    {
        Eat(TokenType::CloseBracket);

        auto target = Factor();

        return std::make_shared<Nodes::CastNode>(typ, target, typ->GetLocation());
    }

    bool Parser::IsTypeCastAhead()
    {
        m_lexer->SaveState();

        if (m_lexer->GetToken()->GetType() != TokenType::Id)
        {
            return false;
        }

        while (m_lexer->GetToken()->GetType() != TokenType::CloseBracket && m_lexer->GetToken()->GetType() != TokenType::_EOF)
        {
            Eat(m_lexer->GetToken()->GetType());
        }

        Eat(m_lexer->GetToken()->GetType());

        auto curr = m_lexer->GetToken()->GetType();

        m_lexer->RestoreState();

        return curr == TokenType::Id || curr == TokenType::OpenBracket;
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

