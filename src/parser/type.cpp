#include "node/type.hpp"
#include "parser.h"
#include "token.h"

namespace AST
{
    Nodes::TypeNode* Parser::Type()
    {
        auto target = Id();

        auto typ = m_arena.Alloc<Nodes::TypeNode>(target, target->GetLocation());

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

    Nodes::TypeNode* Parser::Type(Nodes::IdentifierNode* target)
    {
        auto typ = m_arena.Alloc<Nodes::TypeNode>(target, target->GetLocation());

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

    Nodes::TemplateGeneric* Parser::TmplGeneric()
    {
        auto currToken = m_lexer->GetToken()->GetType();
        if (currToken == TokenType::Comma)
        {
            Eat(TokenType::Comma);
        }

        Eat(TokenType::Question);
        auto genericNode = Id();
        auto generic = m_arena.Alloc<Nodes::TemplateGeneric>(genericNode->GetName(), genericNode->GetLocation());
        return generic;
    }

    Nodes::CastNode* Parser::Cast(Nodes::TypeNode* typ)
    {
        Eat(TokenType::CloseBracket);

        auto target = Factor();

        return m_arena.Alloc<Nodes::CastNode>(typ, target, typ->GetLocation());
    }

    bool Parser::IsTypeCastAhead()
    {
        m_lexer->SaveState();

        if (m_lexer->GetToken()->GetType() != TokenType::Id)
        {
            m_lexer->RestoreState();
            return false;
        }

        Eat(TokenType::Id);

        if (m_lexer->GetToken()->GetType() == TokenType::Less)
        {
            if (!ParseGenericType())
            {
                m_lexer->RestoreState();
                return false;
            }
        }

        if (m_lexer->GetToken()->GetType() != TokenType::CloseBracket)
        {
            m_lexer->RestoreState();
            return false;
        }

        Eat(m_lexer->GetToken()->GetType());

        auto curr = m_lexer->GetToken()->GetType();

        m_lexer->RestoreState();

        return m_lexer->OneOf({
            TokenType::Integer,
            TokenType::Double,
            TokenType::Float,
            TokenType::String,
            TokenType::OpenBracket,
            TokenType::Id
        }, curr);
    }

    Nodes::TypeDfNode* Parser::TypeDfStatement()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::TypeDf);

        auto typName = Id();

        auto typDf = m_arena.Alloc<Nodes::TypeDfNode>(typName, loc);

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

