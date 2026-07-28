
#include "parser.h"
#include "node/var_declaration.hpp"

namespace AST
{
	Node* Parser::VariableDeclaration()
	{
        auto keyword = m_lexer->GetToken();
		bool editable = keyword->GetType() == TokenType::Var;
        auto varLoc = keyword->GetLocation();

		if (editable)
			Eat(TokenType::Var);
		else
			Eat(TokenType::Const);

		Nodes::TypeNode* type = Type();

		Nodes::IdentifierNode* nameNode = Id();
		std::string* name = m_arena.Alloc<std::string>(nameNode->GetName());

		Node* value = nullptr;

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
			return m_arena.Alloc<Nodes::VarDeclaration>(type, name, value, editable, varLoc);
		else
			return m_arena.Alloc<Nodes::VarDeclaration>(type, name, varLoc);
	}
}

