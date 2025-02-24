
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
		Eat(TokenType::OpenBracket);
		std::vector<std::shared_ptr<Node>> args = std::vector<std::shared_ptr<Node>>();
		while (m_lexer->GetToken()->GetType() != TokenType::CloseBracket && m_lexer->GetToken()->GetType() != TokenType::_EOF)
		{
			std::shared_ptr<Node> arg = Expr();
			args.push_back(arg);
			if (m_lexer->GetToken()->GetType() == TokenType::Comma)
			{
				Eat(TokenType::Comma);
			}
		}
		Eat(TokenType::CloseBracket);
		return std::make_shared<Nodes::FunctionCall>(callee, args, callee->GetLocation());
	}

	std::shared_ptr<Nodes::ListNode> Parser::List()
	{
		std::shared_ptr<Nodes::ListNode> list = std::make_shared<Nodes::ListNode>(m_lexer->GetToken()->GetLocation());
		Eat(TokenType::OpenSquareBracket);
		
		while (m_lexer->GetToken()->GetType() != TokenType::CloseSquareBracket && m_lexer->GetToken()->GetType() != TokenType::_EOF)
		{
			std::shared_ptr<Node> item = Ternary();
			list->AddItem(item);
			if (m_lexer->GetToken()->GetType() == TokenType::Comma)
			{
				Eat(TokenType::Comma);
			}
		}

		Eat(TokenType::CloseSquareBracket);

		return list;
	}
}
