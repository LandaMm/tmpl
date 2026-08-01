#include <memory>
#include <cassert>

#include "basics/array.hpp"
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

		// 2. params
		Eat(TokenType::OpenBracket);

		auto currToken = m_lexer->GetToken()->GetType();

		Array<Nodes::FunctionParam*> fnParams;
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
			fnParams.Push(param);
			currToken = m_lexer->GetToken()->GetType();
		}

		Eat(TokenType::CloseBracket);

		Eat(TokenType::SingleArrow);

		Nodes::TypeNode* retType = Type();

		Nodes::SymbolFlag fnFlag = Nodes::SymbolFlag::NONE;
		// Symbol Flag
		// (none) | #foreign
		if (m_lexer->GetToken()->GetType() == TokenType::Hash)
		{
			Eat(TokenType::Hash);

			auto nextToken = m_lexer->GetToken();
			assert(static_cast<int>(Nodes::SymbolFlag::COUNT_SYMBOL_FLAGS) == 2);

			switch (nextToken->GetType())
			{
			case TokenType::Foreign:
				Eat(TokenType::Foreign);

				fnFlag = Nodes::SymbolFlag::FOREIGN;
				break;
			default:
			{
				auto &errManager = GetErrorManager();
				errManager.UnexpectedToken(m_lexer->GetFilename(), nextToken);
				break;
			}
			}
		}

		Statements::StatementsBody* body =
			m_arena.Alloc<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());

		Nodes::FunctionDeclaration* fn =
			m_arena.Alloc<Nodes::FunctionDeclaration>(fnName, body, fnFlag, fnLoc);

		for (auto param : fnParams)
		{
			fn->AddParam(param);
		}
		fn->SetReturnType(retType);

		return fn;
    }

    Node* Parser::FunctionDeclaration()
    {
        // fn name(type param, type param2) : type {...}
        auto fn = FunctionSignature();

		assert(static_cast<int>(Nodes::SymbolFlag::COUNT_SYMBOL_FLAGS) == 2);
		if (fn->GetSymbolFlag() == Nodes::SymbolFlag::FOREIGN)
		{
			// it is an externed function which should not have a body.
			return fn;
		}

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
