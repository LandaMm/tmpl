

#include <memory>
#include "cpl-parser/parser.h"
#include "cpl-parser/node/directive.hpp"

namespace AST
{
    using namespace AST::Nodes;

    Node* Parser::ImportStatement()
    {
        auto loc = Current()->GetLocation();
        Eat(TokenType::Import);

        auto token = Current();
        Eat(TokenType::String);
        auto module = token->GetValue<String>();
        assert(module);

        return m_arena.Alloc<ImportDirective>(*module, loc);
    }

    Node* Parser::ExternStatement()
    {
        auto loc = Current()->GetLocation();
        Eat(TokenType::Extern);

        // TODO: maybe add support for static variables from c
        auto fnSign = FunctionSignature();
        Eat(TokenType::Semicolon);

        return m_arena.Alloc<ExternDirective>(fnSign, loc);
    }
}

