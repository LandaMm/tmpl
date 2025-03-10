

#include "../../include/parser.h"
#include "../../include/node/object_member.h"

namespace AST
{
	std::shared_ptr<Node> Parser::ObjectMember(std::shared_ptr<Node> obj)
	{
		std::shared_ptr<Node> result = nullptr;
		std::shared_ptr<Nodes::ObjectMember> objMember = std::make_shared<Nodes::ObjectMember>(obj, obj->GetLocation());

		do
		{
			// shell["exec"]
			// shell.exec
			// shell[5]
			// shell["pty"]["exec"]._version
			if (m_lexer->GetToken()->GetType() == TokenType::Point)
			{
				Eat(TokenType::Point);
				std::shared_ptr<Node> member = Id();
				objMember->SetMember(member);
                objMember->SetLocation(member->GetLocation());
				result = objMember;
				objMember = std::make_shared<Nodes::ObjectMember>(result, result->GetLocation());
			}
			else if (m_lexer->GetToken()->GetType() == TokenType::OpenSquareBracket)
			{
				Eat(TokenType::OpenSquareBracket);
				std::shared_ptr<Node> member = Expr();
				Eat(TokenType::CloseSquareBracket);
				objMember->SetMember(member);
                objMember->SetLocation(member->GetLocation());
				result = objMember;
				objMember = std::make_shared<Nodes::ObjectMember>(result, result->GetLocation());
			}
			else
			{
				GetErrorManager().UnexpectedToken(m_lexer->GetFilename(), m_lexer->GetToken());
			}
		} while (m_lexer->GetToken()->GetType() == TokenType::Point || m_lexer->GetToken()->GetType() == TokenType::OpenSquareBracket);

		return result;
	}
}
