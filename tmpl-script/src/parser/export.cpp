
#include "include/parser.h"
#include "include/node/export.h"

namespace AST
{
    std::shared_ptr<Node> Parser::ExportStmt()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::Export);

        std::shared_ptr<Node> target = nullptr;

        auto token = m_lexer->GetToken();
        switch (token->GetType())
        {
            case TokenType::Fn:
            {
                target = FunctionDeclaration();
                std::shared_ptr<Nodes::FunctionDeclaration> test = std::dynamic_pointer_cast<Nodes::FunctionDeclaration>(target);
                break;
            }
            case TokenType::Const:
                target = VariableDeclaration();
                Eat(TokenType::Semicolon);
                break;
            case TokenType::TypeDf:
                target = TypeDfStatement();
                Eat(TokenType::Semicolon);
                break;
            default:
                Prelude::ErrorManager &manager = GetErrorManager();
                manager.UnexpectedToken(GetFilename(), token);
                return nullptr;
        }

        assert(target != nullptr && "Export target shouldn't be left null");

        return std::make_shared<Nodes::ExportStatement>(target, loc);
    }
}

