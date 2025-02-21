
#include "include/node/procedure.h"
#include "include/parser.h"

namespace AST
{
	std::shared_ptr<Node> Parser::ProcedureDeclaration()
	{
        auto procLoc = m_lexer->GetToken()->GetLocation();
		Eat(TokenType::SingleArrow);
		std::shared_ptr<Nodes::IdentifierNode> nameId = Id();

		std::shared_ptr<Statements::StatementsBody> body =
			std::make_shared<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());

		Eat(TokenType::OpenCurly);
		while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
		{
			std::shared_ptr<Node> statement = Statement();
			body->AddItem(statement);
		}
		Eat(TokenType::CloseCurly);

		auto procedure =
			std::make_shared<Nodes::ProcedureDeclaration>(nameId->GetName(), body, procLoc);

		return procedure;
	}
}

