#include <map>

#include "cpl-parser/node/type.hpp"
#include "cpl-parser/node/literal.hpp"
#include "cpl-parser/parser.h"
#include "cpl-parser/token.h"

namespace AST
{
    Nodes::Type* Parser::Type()
    {
        // type_name|*type_name|struct {..}|enum {..}

        auto token = Current();

        switch (token->GetType())
        {
        case TokenType::Id:
        {
            auto typeName = Id();
            return m_arena.Alloc<Nodes::BasicType>(typeName->GetName(), typeName->GetLocation());
        }
        case TokenType::Multiply:
        {
            Eat(TokenType::Multiply);
            auto targetType = Type();
            LocationSpan loc = targetType->GetLocation();
            loc.begin = token->GetLocation().begin; // include '*' as well
            return m_arena.Alloc<Nodes::PointerType>(targetType, loc);
        }
        case TokenType::Struct:
        {
            Eat(TokenType::Struct);
            Eat(TokenType::OpenCurly);

            // struct {
            //   name: type;
            //   ..
            // }
            std::map<String, const Nodes::Type*> fields;
            while (Current()->Not(TokenType::CloseCurly))
            {
                auto fieldName = Id();
                Eat(TokenType::Colon);
                auto fieldType = Type();
                Eat(TokenType::Semicolon);
                fields.insert({ fieldName->GetName(), fieldType });
            }

            Eat(TokenType::CloseCurly);

            // TODO: maybe provide loc span of the whole struct including the body
            auto loc = token->GetLocation();
            return m_arena.Alloc<Nodes::StructType>(std::move(fields), loc);
        }
        case TokenType::Enum:
        {
            Eat(TokenType::Enum);
            Eat(TokenType::OpenCurly);

            // enum {
            //   | name = value;
            //   | name;
            //   ..
            // }
            auto enumType = m_arena.Alloc<Nodes::EnumType>(token->GetLocation());
            while (Current()->Not(TokenType::CloseCurly))
            {
                auto fieldName = Id();

                Uint64 fieldValue = 0;
                if (Current()->Is(TokenType::Equal))
                {
					Eat(TokenType::Equal);
                    auto value = Expr();
                    fieldValue = EvaluateIntegerConstantExpression(value);
                }
                else if (enumType->GetFields().Size() > 0)
                {
                    fieldValue = enumType->GetFields()[enumType->GetFields().Size() - 1].value + 1;
                }

                Eat(TokenType::Semicolon);

				enumType->AddField(Nodes::EnumField{ fieldName->GetName(), fieldValue });
            }

            Eat(TokenType::CloseCurly);

            // TODO: maybe provide loc span of the whole enum including the body
            auto loc = token->GetLocation();
            return enumType;
        }
        }

        // TODO: better error
        assert(false && "Invalid type");
        return nullptr;
    }

    Nodes::CastNode* Parser::Cast(Nodes::Type* typ)
    {
        Eat(TokenType::CloseBracket);

        auto target = Factor();

        return m_arena.Alloc<Nodes::CastNode>(typ, target, typ->GetLocation());
    }

    bool Parser::IsTypeCastAhead()
    {
        CurrentLexer()->SaveState();

        if (Current()->Not(TokenType::Id))
        {
            CurrentLexer()->RestoreState();
            return false;
        }

        Eat(TokenType::Id);

        if (Current()->Not(TokenType::CloseBracket))
        {
            CurrentLexer()->RestoreState();
            return false;
        }

        Advance();

        auto curr = Current()->GetType();

        CurrentLexer()->RestoreState();

		return Current()->OneOf({
			TokenType::Integer,
			TokenType::Double,
			TokenType::Float,
			TokenType::String,
			TokenType::OpenBracket,
			TokenType::Id
		});
    }

	Nodes::TypeDeclaration* Parser::TypeDeclaration()
    {
        auto loc = Current()->GetLocation();

        auto typName = Id();

        auto typDeclaration = m_arena.Alloc<Nodes::TypeDeclaration>(typName, loc);
        
        Eat(TokenType::DoubleColon);

        auto value = Type();

        typDeclaration->SetValue(value);

        return typDeclaration;
    }
}

