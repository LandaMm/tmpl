#include "node/type.hpp"
#include "parser.h"
#include "token.h"

namespace AST
{
    Nodes::TypeNode* Parser::Type()
    {
        auto target = Id();

        auto typ = m_arena.Alloc<Nodes::TypeNode>(target, target->GetLocation());

        return typ;
    }

    Nodes::TypeNode* Parser::Type(Nodes::IdentifierNode* target)
    {
        auto typ = m_arena.Alloc<Nodes::TypeNode>(target, target->GetLocation());
        return typ;
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

	Nodes::TypeDeclaration* Parser::TypeDeclaration()
    {
        auto loc = m_lexer->GetToken()->GetLocation();

        auto typName = Id();

        auto typDeclaration = m_arena.Alloc<Nodes::TypeDeclaration>(typName, loc);
        
        Eat(TokenType::DoubleColon);

        auto value = Type();

        typDeclaration->SetValue(value);

        return typDeclaration;
    }
}

