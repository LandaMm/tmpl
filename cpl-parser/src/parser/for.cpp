
#include "cpl-parser/parser.h"
#include "cpl-parser/node/loop.hpp"

namespace AST
{
    Node* Parser::ForLoop()
    {
        auto loc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::For);

        Eat(TokenType::OpenBracket);

        auto decl = VariableDeclaration();
        Eat(TokenType::Semicolon);

        // TODO: think if Condition() is better
        auto cond = Ternary();
        Eat(TokenType::Semicolon);

        auto assignment = Assignment();
        
        Eat(TokenType::CloseBracket);

        Statements::StatementsBody* body =
            m_arena.Alloc<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());

        m_breaks.push_back(loc);

        if (m_lexer->GetToken()->GetType() != TokenType::OpenCurly)
        {
			body->AddItem(Statement());
        }
        else
        {
            Eat(TokenType::OpenCurly);
            while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
            {
                Node* statement = Statement();
                body->AddItem(statement);
            }
            Eat(TokenType::CloseCurly);
        }

        m_breaks.pop_back();

        return m_arena.Alloc<Nodes::ForLoopNode>(decl, cond, assignment, body, loc);
    }
}

