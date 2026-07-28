#include <memory>
#include <cassert>

#include "node/identifier.hpp"
#include "parser.h"
#include "token.h"
#include "node/function.hpp"

namespace AST
{
    Nodes::FunctionDeclaration* Parser::FunctionSignature()
    {
		// name :: (param: type, param2: type) : type
		auto fnName = Id();
		auto fnLoc = fnName->GetLocation();

		Eat(TokenType::DoubleColon);

		Statements::StatementsBody* body =
			m_arena.Alloc<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());

		Nodes::FunctionDeclaration* fn =
			m_arena.Alloc<Nodes::FunctionDeclaration>(fnName, body, fnLoc);

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
			Nodes::IdentifierNode* name = Id();
			Eat(TokenType::Colon);
			Nodes::TypeNode* type = Type();
			Nodes::FunctionParam* param = m_arena.Alloc<Nodes::FunctionParam>(type, name);
			fn->AddParam(param);
			currToken = m_lexer->GetToken()->GetType();
		}

		Eat(TokenType::CloseBracket);

		Eat(TokenType::SingleArrow);

		Nodes::TypeNode* retType = Type();

		fn->SetReturnType(retType);

		return fn;
    }

    Node* Parser::FunctionDeclaration()
    {
        // fn name(type param, type param2) : type {...}
        auto fn = FunctionSignature();
        auto body = fn->GetBody();

        Eat(TokenType::OpenCurly);

        while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
        {
            Node* statement = Statement();
            body->AddItem(statement);
        }

        Eat(TokenType::CloseCurly);

        return fn;
    }
}
