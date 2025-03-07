
#include"../../include/parser.h"
#include"../../include/node/identifier.h"

namespace AST
{
	std::shared_ptr<Nodes::IdentifierNode> Parser::Id()
	{
		auto token = m_lexer->GetToken();
		Eat(TokenType::Id);
		std::shared_ptr<std::string> name = token->GetValue<std::string>();
		return std::make_shared<Nodes::IdentifierNode>(name->c_str(), token->GetLocation());
	}

	std::shared_ptr<Nodes::FunctionCall> Parser::FunctionCall(std::shared_ptr<Node> callee)
	{
        auto fnCall = std::make_shared<Nodes::FunctionCall>(callee, callee->GetLocation());

        if (m_lexer->GetToken()->GetType() == TokenType::Less)
        {
            Eat(TokenType::Less);
            while (m_lexer->GetToken()->GetType() != TokenType::Greater)
            {
                fnCall->AddGeneric(Type());
            }
            Eat(TokenType::Greater);
        }

		Eat(TokenType::OpenBracket);
		while (m_lexer->GetToken()->GetType() != TokenType::CloseBracket && m_lexer->GetToken()->GetType() != TokenType::_EOF)
		{
			std::shared_ptr<Node> arg = Expr();
            fnCall->AddArgument(arg);
			if (m_lexer->GetToken()->GetType() == TokenType::Comma)
			{
				Eat(TokenType::Comma);
			}
		}
		Eat(TokenType::CloseBracket);

		return fnCall;
	}

	std::shared_ptr<Nodes::ListNode> Parser::List()
	{
		Eat(TokenType::OpenSquareBracket);
        Eat(TokenType::CloseSquareBracket);

        auto typ = Type();

		std::shared_ptr<Nodes::ListNode> list = std::make_shared<Nodes::ListNode>(typ, m_lexer->GetToken()->GetLocation());

        Eat(TokenType::OpenCurly);
		
		while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
		{
			std::shared_ptr<Node> item = Ternary();
			list->AddItem(item);
			if (m_lexer->GetToken()->GetType() == TokenType::Comma)
			{
				Eat(TokenType::Comma);
			}
		}

		Eat(TokenType::CloseCurly);

		return list;
	}
}
