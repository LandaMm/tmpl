#include <memory>
#include <cassert>

#include "cpl-basics/array.hpp"
#include "cpl-parser/node/identifier.hpp"
#include "cpl-parser/parser.h"
#include "cpl-parser/token.h"
#include "cpl-parser/node/function.hpp"

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

		Array<Nodes::FunctionParam*> fnParams;
		while (Current()->Not(TokenType::CloseBracket))
		{
			// TODO: think if we should allow trailing comma in function signature
			if (Current()->Is(TokenType::Comma))
			{
				Eat(TokenType::Comma);
			}
			Nodes::IdentifierNode* name = Id();
			Eat(TokenType::Colon);
			auto type = Type();
			Nodes::FunctionParam* param = m_arena.Alloc<Nodes::FunctionParam>(type, name);
			fnParams.Push(param);
		}

		Eat(TokenType::CloseBracket);

		Eat(TokenType::SingleArrow);

		auto retType = Type();

		Nodes::SymbolFlag fnFlag = Nodes::SymbolFlag::NONE;
		// Symbol Flag
		// (none) | #foreign
		if (Current()->Is(TokenType::Hash))
		{
			Eat(TokenType::Hash);

			auto nextToken = Current();
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
			m_arena.Alloc<Statements::StatementsBody>(Current()->GetLocation());

		Nodes::FunctionDeclaration* fn =
			m_arena.Alloc<Nodes::FunctionDeclaration>(fnName, body, fnFlag, fnLoc);

		for (auto param : fnParams)
		{
			fn->AddParam(param);
		}
		fn->SetReturnType(retType);

		return fn;
    }

    Nodes::FunctionDeclaration* Parser::FunctionDeclaration()
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

        while (Current()->Not(TokenType::CloseCurly))
        {
            Node* statement = Statement();
            body->AddItem(statement);
        }

        Eat(TokenType::CloseCurly);

        return fn;
    }
}
