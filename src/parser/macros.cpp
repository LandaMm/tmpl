

#include <memory>
#include "../../include/parser.h"
#include "../../include/node/macros.h"

namespace AST
{
    using namespace AST::Nodes;

    std::shared_ptr<Node> Parser::RequireStatement()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::Require);

        auto token = m_lexer->GetToken();
        Eat(TokenType::String);
        std::shared_ptr<std::string> module = token->GetValue<std::string>();

        return std::make_shared<RequireMacro>(*module, loc);
    }

    std::shared_ptr<Node> Parser::ExternStatement()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::Extern);

        // TODO: maybe add support for static variables from c
        auto fnSign = FunctionSignature();
        Eat(TokenType::Semicolon);

        return std::make_shared<ExternMacro>(fnSign, loc);
    }
}

