#include <memory>
#include <cassert>

#include "node/identifier.hpp"
#include "parser.h"
#include "token.h"
#include "node/function.hpp"

namespace AST
{
    std::shared_ptr<Nodes::FunctionDeclaration> Parser::FunctionSignature()
    {
		// name :: (param: type, param2: type) : type
		auto fnName = Id();
		auto fnLoc = fnName->GetLocation();

		Eat(TokenType::DoubleColon);

		std::shared_ptr<Statements::StatementsBody> body =
			std::make_shared<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());

		std::shared_ptr<Nodes::FunctionDeclaration> fn =
			std::make_shared<Nodes::FunctionDeclaration>(fnName, body, fnLoc);

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
			std::shared_ptr<Nodes::IdentifierNode> name = Id();
			Eat(TokenType::Colon);
			std::shared_ptr<Nodes::TypeNode> type = Type();
			std::shared_ptr<Nodes::FunctionParam> param = std::make_shared<Nodes::FunctionParam>(type, name);
			fn->AddParam(param);
			currToken = m_lexer->GetToken()->GetType();
		}

		Eat(TokenType::CloseBracket);

		Eat(TokenType::SingleArrow);

		std::shared_ptr<Nodes::TypeNode> retType = Type();

		fn->SetReturnType(retType);

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
