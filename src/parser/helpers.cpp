
#include "include/parser.h"

namespace AST
{
    bool Parser::ParseGenericType()
    {
        Eat(TokenType::Less);  // Consume '<'

        int depth = 1;  // Track nested generics like `box<vector<int>>`
        while (depth > 0)
        {
            auto tok = m_lexer->GetToken();
            if (tok->GetType() == TokenType::Less)
            {
                depth++;
            }
            else if (tok->GetType() == TokenType::Greater)
            {
                depth--;
            }
            else if (tok->GetType() != TokenType::Id && tok->GetType() != TokenType::Comma)
            {
                return false;  // Invalid generic type syntax
            }
            Eat(tok->GetType());
        }

        return true;  // Successfully parsed generic type
    }
}

