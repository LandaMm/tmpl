
#include "parser.h"
#include "node/identifier.hpp"
#include "token.h"
#include "cpl-basics/string.hpp"

namespace AST
{
	Nodes::IdentifierNode* Parser::Id()
	{
		auto token = m_lexer->GetToken();
		Eat(TokenType::Id);
		auto name = token->GetValue<String>();
		return m_arena.Alloc<Nodes::IdentifierNode>(name->c_str(), token->GetLocation());
	}

	Nodes::FunctionCall* Parser::FunctionCall(Node* callee)
	{
        auto fnCall = m_arena.Alloc<Nodes::FunctionCall>(callee, callee->GetLocation());

		Eat(TokenType::OpenBracket);
		while (m_lexer->GetToken()->GetType() != TokenType::CloseBracket && m_lexer->GetToken()->GetType() != TokenType::_EOF)
		{
			Node* arg = Expr();
            fnCall->AddArgument(arg);
			if (m_lexer->GetToken()->GetType() == TokenType::Comma)
			{
				Eat(TokenType::Comma);
			}
		}
		Eat(TokenType::CloseBracket);

		return fnCall;
	}
}
