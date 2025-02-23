
#include "include/parser.h"
#include "include/node/loop.h"

namespace AST
{
    std::shared_ptr<Node> Parser::WhileLoop()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::While);

        Eat(TokenType::OpenBracket);
        auto condition = Ternary();
        Eat(TokenType::CloseBracket);

        std::shared_ptr<Statements::StatementsBody> body =
            std::make_shared<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());

        if (m_lexer->GetToken()->GetType() != TokenType::OpenCurly)
        {
			body->AddItem(Statement());
        }
        else
        {
            Eat(TokenType::OpenCurly);
            while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
            {
                std::shared_ptr<Node> statement = Statement();
                body->AddItem(statement);
            }
            Eat(TokenType::CloseCurly);
        }

        return std::make_shared<Nodes::WhileNode>(condition, body, loc);
    }
}

