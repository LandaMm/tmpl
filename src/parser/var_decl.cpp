
#include "parser.h"
#include "node/var_declaration.hpp"

namespace AST
{
	Node* Parser::VariableDeclaration()
	{
		Nodes::IdentifierNode* nameNode = Id();
        auto varLoc = nameNode->GetLocation();

		std::string* name = m_arena.Alloc<std::string>(nameNode->GetName());

		Eat(TokenType::Colon);

		Nodes::TypeNode* type = Type();

		Nodes::SymbolFlag symbolFlag = Nodes::SymbolFlag::NONE;
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

				symbolFlag = Nodes::SymbolFlag::FOREIGN;
				break;
			default:
			{
				auto &errManager = GetErrorManager();
				errManager.UnexpectedToken(m_lexer->GetFilename(), nextToken);
				break;
			}
			}
		}

		Node* value = nullptr;

		if (m_lexer->GetToken()->GetType() == TokenType::Equal)
		{
			// Default values are not allowed for externed symbols
			if (symbolFlag == Nodes::SymbolFlag::FOREIGN)
			{
				// TODO: better error
				assert(false && "Default value on a foreign variable declaration is not allowed");
			}
			Eat(TokenType::Equal);
			value = Ternary();
		}

		return m_arena.Alloc<Nodes::VariableDeclaration>(type, name, value, varLoc);
	}
}

