

#include <memory>
#include "parser.h"
#include "node/directive.hpp"

namespace AST
{
    using namespace AST::Nodes;

    Node* Parser::ImportStatement()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::Import);

        auto token = m_lexer->GetToken();
        Eat(TokenType::String);
        auto module = token->GetValue<String>();
        assert(module);

        return m_arena.Alloc<ImportDirective>(*module, loc);
    }

    Node* Parser::ExternStatement()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::Extern);

        // TODO: maybe add support for static variables from c
        auto fnSign = FunctionSignature();
        Eat(TokenType::Semicolon);

        return m_arena.Alloc<ExternDirective>(fnSign, loc);
    }
}

