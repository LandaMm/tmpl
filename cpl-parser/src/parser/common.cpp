
#include "cpl-parser/parser.h"
#include "cpl-parser/node/identifier.hpp"
#include "cpl-parser/token.h"
#include "cpl-basics/string.hpp"

namespace AST
{
	Nodes::IdentifierNode* Parser::Id()
	{
		auto token = Current();
		Eat(TokenType::Id);
		auto name = token->GetValue<String>();
		return m_arena.Alloc<Nodes::IdentifierNode>(name->c_str(), token->GetLocation());
	}

	Nodes::FunctionCall* Parser::FunctionCall(Node* callee)
	{
        auto fnCall = m_arena.Alloc<Nodes::FunctionCall>(callee, callee->GetLocation());

		Eat(TokenType::OpenBracket);
		while (Current()->GetType() != TokenType::CloseBracket && Current()->GetType() != TokenType::_EOF)
		{
			Node* arg = Expr();
            fnCall->AddArgument(arg);
			if (Current()->GetType() == TokenType::Comma)
			{
				Eat(TokenType::Comma);
			}
		}
		Eat(TokenType::CloseBracket);

		return fnCall;
	}
}
