#include "include/node/type.h"
#include "include/parser.h"
#include "include/token.h"

namespace AST
{
    std::shared_ptr<Nodes::TypeNode> Parser::Type()
    {
        auto target = Id();

        auto typ = std::make_shared<Nodes::TypeNode>(target, target->GetLocation());

        if (m_lexer->GetToken()->GetType() == TokenType::Less)
        {
            Eat(TokenType::Less);
            auto currToken = m_lexer->GetToken()->GetType();
            while (currToken != TokenType::Greater)
            {
                if (currToken == TokenType::Comma)
                {
                    Eat(TokenType::Comma);
                }

                typ->AddGenericType(Type());

                currToken = m_lexer->GetToken()->GetType();
            }
            Eat(TokenType::Greater);
        }

        return typ;
    }

    std::shared_ptr<Nodes::TypeNode> Parser::Type(std::shared_ptr<Nodes::IdentifierNode> target)
    {
        auto typ = std::make_shared<Nodes::TypeNode>(target, target->GetLocation());

        if (m_lexer->GetToken()->GetType() == TokenType::Less)
        {
            Eat(TokenType::Less);
            auto currToken = m_lexer->GetToken()->GetType();
            while (currToken != TokenType::Greater)
            {
                if (currToken == TokenType::Comma)
                {
                    Eat(TokenType::Comma);
                }

                typ->AddGenericType(Type());

                currToken = m_lexer->GetToken()->GetType();
            }
            Eat(TokenType::Greater);
        }

        return typ;
    }

    std::shared_ptr<Nodes::TemplateGeneric> Parser::TmplGeneric()
    {
        auto currToken = m_lexer->GetToken()->GetType();
        if (currToken == TokenType::Comma)
        {
            Eat(TokenType::Comma);
        }

        Eat(TokenType::Question);
        auto genericNode = Id();
        auto generic = std::make_shared<Nodes::TemplateGeneric>(genericNode->GetName(), genericNode->GetLocation());
        return generic;
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

        auto typName = Id();

        auto typDf = std::make_shared<Nodes::TypeDfNode>(typName, loc);

        if (m_lexer->GetToken()->GetType() == TokenType::Less)
        {
            Eat(TokenType::Less);
            while (m_lexer->GetToken()->GetType() != TokenType::Greater)
            {
                typDf->AddGeneric(TmplGeneric());
            }
            Eat(TokenType::Greater);
        }
        
        Eat(TokenType::Equal);

        auto value = Type();

        typDf->SetValue(value);

        return typDf;
    }
}

