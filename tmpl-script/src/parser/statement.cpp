
#include "../../include/parser.h"
#include "../../include/node/statement.h"
#include "../../include/node/var_declaration.h"
#include "../../include/node/procedure.h"

namespace AST
{
	std::shared_ptr<Node> Parser::ProcedureDeclaration()
	{
		Eat(TokenType::SingleArrow);
		std::shared_ptr<Nodes::IdentifierNode> nameId = Id();

		std::shared_ptr<Nodes::ProcedureDeclaration> procedure =
			std::make_shared<Nodes::ProcedureDeclaration>(nameId->GetName());

		Eat(TokenType::OpenCurly);
		while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
		{
			std::shared_ptr<Node> statement = Statement();
			procedure->AddItem(statement);
		}
		Eat(TokenType::CloseCurly);

		return procedure;
	}

	std::shared_ptr<Node> Parser::VariableDeclaration()
	{
		bool editable = m_lexer->GetToken()->GetType() == TokenType::Var;
		if (editable)
			Eat(TokenType::Var);
		else
			Eat(TokenType::Const);

		std::shared_ptr<Node> type = Id();

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
			manager.MissingConstantDefinition(m_lexer->GetToken());
			return nullptr;
		}

		if (value != nullptr)
			return std::make_shared<Nodes::VarDeclaration>(type, name, value, editable);
		else
			return std::make_shared<Nodes::VarDeclaration>(type, name);
	}

	std::shared_ptr<Node> Parser::IfElseStatement()
	{
		// if (5 == 5 ? true : false) {} else {}
		Eat(TokenType::If);
		// Eat(TokenType::OpenBracket)
		std::shared_ptr<Node> condition = Ternary();
		// Eat(TokenType::CloseBracket)

		auto ifElse = std::make_shared<Statements::IfElseStatement>(condition);

		if (m_lexer->GetToken()->GetType() != TokenType::OpenCurly)
		{
			ifElse->AddItem(Statement());
		}
		else
		{
			Eat(TokenType::OpenCurly);
			while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
			{
				std::shared_ptr<Node> statement = Statement();
				ifElse->AddItem(statement);
			}
			Eat(TokenType::CloseCurly);
		}

		if (m_lexer->GetToken()->GetType() == TokenType::Else)
		{
			Eat(TokenType::Else);

			if (m_lexer->GetToken()->GetType() == TokenType::If)
			{
				std::shared_ptr<Node> elseNode = IfElseStatement();
				ifElse->SetElseStatement(elseNode);
			}
			else
			{
				auto stmts = std::make_shared<Statements::StatementsBody>();
				if (m_lexer->GetToken()->GetType() != TokenType::OpenCurly)
				{
					stmts->AddItem(Statement());
				}
				else
				{
					Eat(TokenType::OpenCurly);
					while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
					{
						std::shared_ptr<Node> statement = Statement();
						stmts->AddItem(statement);
					}
					Eat(TokenType::CloseCurly);
				}
				ifElse->SetElseStatement(stmts);
			}
		}

		return ifElse;
	}
}
