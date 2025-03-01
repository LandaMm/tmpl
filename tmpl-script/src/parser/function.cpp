#include "include/node/identifier.h"
#include "include/parser.h"
#include "include/token.h"
#include <memory>
#include "include/node/function.h"

namespace AST
{
    std::shared_ptr<Nodes::FunctionDeclaration> Parser::FunctionSignature()
    {
        // fn name(type param, type param2) : type
        auto fnLoc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::Fn);

        Nodes::FunctionModifier modifier = Nodes::FunctionModifier::None;

        if (m_lexer->GetToken()->GetType() == TokenType::At)
        {
            auto atToken = m_lexer->GetToken();
            Eat(TokenType::At);

            switch(m_lexer->GetToken()->GetType())
            {
                case TokenType::Construct:
                    Eat(TokenType::Construct);
                    modifier = Nodes::FunctionModifier::Construct;
                    break;
                case TokenType::Cast:
                    Eat(TokenType::Cast);
                    modifier = Nodes::FunctionModifier::Cast;
                    break;
                default:
                {
                    Prelude::ErrorManager& errManager = GetErrorManager();
                    errManager.UnexpectedFnModifier(GetFilename(), m_lexer->GetToken(), atToken->GetLocation());
                    return nullptr;
                }
            }
        }

        std::shared_ptr<Node> fnName;

        // fn List<?T>.addItem() {...}
        // fn sayHi<?T>() {...}
        // fn List<?T>.castItem<?K>() {...}
        // fn @construct List<?T>() {...} // DONE
        // fn @cast(List<?T>) : set<T> {...} // DONE
        
        if (modifier == Nodes::FunctionModifier::Construct)
        {
            fnName = Id();
        }
        else if (modifier == Nodes::FunctionModifier::Cast)
        {
            Eat(TokenType::OpenBracket);
            fnName = TypeTemplate(Id());
            Eat(TokenType::CloseBracket);
        }
        else // No modifier
        {
            fnName = Id();
            bool isTypFn = false;
            if (m_lexer->GetToken()->GetType() == TokenType::Less)
            {
                m_lexer->SaveState();
                while(m_lexer->GetToken()->GetType() != TokenType::Greater && m_lexer->GetToken()->GetType() != TokenType::_EOF)
                {
                    Eat(m_lexer->GetToken()->GetType());
                }
                Eat(TokenType::Greater);

                if (m_lexer->GetToken()->GetType() == TokenType::Point)
                {
                    isTypFn = true;
                }
                m_lexer->RestoreState();
            }
            else if (m_lexer->GetToken()->GetType() == TokenType::Point)
            {
                isTypFn = true;
            }
            if (isTypFn)
            {
                fnName = TypeTemplate(std::dynamic_pointer_cast<Nodes::IdentifierNode>(fnName));
                fnName = ObjectMember(fnName);
            }
        }

        assert(fnName != nullptr && "Fn name shouldn't be left null in signature");

        std::shared_ptr<Statements::StatementsBody> body =
            std::make_shared<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());

        std::shared_ptr<Nodes::FunctionDeclaration> fn =
            std::make_shared<Nodes::FunctionDeclaration>(fnName, body, modifier, fnLoc);

        if (modifier != Nodes::FunctionModifier::Cast)
        {
            // 1. generics
            if (m_lexer->GetToken()->GetType() == TokenType::Less)
            {
                Eat(TokenType::Less);
                while (m_lexer->GetToken()->GetType() != TokenType::Greater)
                {
                    fn->AddGeneric(TmplGeneric());
                }
                Eat(TokenType::Greater);
            }
        
            // 2. params
            Eat(TokenType::OpenBracket);

            auto currToken = m_lexer->GetToken()->GetType();

            while (currToken != TokenType::CloseBracket
                    && currToken != TokenType::_EOF)
            {
                if (currToken == TokenType::Comma)
                {
                    Eat(TokenType::Comma);
                }
                std::shared_ptr<Nodes::TypeNode> type = Type();
                std::shared_ptr<Nodes::IdentifierNode> name = Id();
                std::shared_ptr<Nodes::FunctionParam> param = std::make_shared<Nodes::FunctionParam>(type, name);
                fn->AddParam(param);
                currToken = m_lexer->GetToken()->GetType();
            }

            Eat(TokenType::CloseBracket);
        }

        if (modifier != Nodes::FunctionModifier::Construct)
        {
            Eat(TokenType::Colon);

            std::shared_ptr<Nodes::TypeNode> retType = Type();

            fn->SetReturnType(retType);
        }

        return fn;
    }

    std::shared_ptr<Node> Parser::FunctionDeclaration()
    {
        // fn name(type param, type param2) : type {...}
        auto fn = FunctionSignature();
        auto body = fn->GetBody();

        Eat(TokenType::OpenCurly);

        while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
        {
            std::shared_ptr<Node> statement = Statement();
            body->AddItem(statement);
        }

        Eat(TokenType::CloseCurly);

        return fn;
    }
}

