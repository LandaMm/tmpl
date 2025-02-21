
#include "include/node/var_declaration.h"
#include "include/parser.h"

namespace AST
{
	std::shared_ptr<Node> Parser::VariableDeclaration()
	{
        auto keyword = m_lexer->GetToken();
		bool editable = keyword->GetType() == TokenType::Var;
        auto varLoc = keyword->GetLocation();

		if (editable)
			Eat(TokenType::Var);
		else
			Eat(TokenType::Const);

		std::shared_ptr<Nodes::TypeNode> type = Type();

		std::shared_ptr<Nodes::IdentifierNode> nameNode = Id();
		std::shared_ptr<std::string> name = std::make_shared<std::string>(nameNode->GetName());

		std::shared_ptr<Node> value = nullptr;

		if (m_lexer->GetToken()->GetType() == TokenType::Equal)
		{
			Eat(TokenType::Equal);
			value = Ternary();
		}
		else if (!editable)
		{
			Prelude::ErrorManager &manager = GetErrorManager();
			manager.MissingConstantDefinition(GetFilename(), m_lexer->GetToken());
			return nullptr;
		}

		if (value != nullptr)
			return std::make_shared<Nodes::VarDeclaration>(type, name, value, editable, varLoc);
		else
			return std::make_shared<Nodes::VarDeclaration>(type, name, varLoc);
	}
}

