
#include "../../include/parser.h"
#include "../../include/node/statement.h"
#include "../../include/node/var_declaration.h"
#include "../../include/node/procedure.h"
#include "../../include/node/return.h"
#include "../../include/node/function.h"
#include <memory>

namespace AST
{
    std::shared_ptr<Node> Parser::FunctionDeclaration()
    {
        // fn name(type param, type param2) : type {...}
        auto fnLoc = m_lexer->GetToken()->GetLocation();
        Eat(TokenType::Fn);

        std::shared_ptr<Nodes::IdentifierNode> name = Id();

        Eat(TokenType::OpenBracket);

        std::shared_ptr<Statements::StatementsBody> body =
            std::make_shared<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());

        std::shared_ptr<Nodes::FunctionDeclaration> fn =
            std::make_shared<Nodes::FunctionDeclaration>(name, body, fnLoc);

        auto currToken = m_lexer->GetToken()->GetType();

        while (currToken != TokenType::CloseBracket
                && currToken != TokenType::_EOF)
        {
            if (currToken == TokenType::Comma)
            {
                Eat(TokenType::Comma);
            }
            // TODO: support for complex types
            std::shared_ptr<Node> type = Id();
            std::shared_ptr<Nodes::IdentifierNode> name = Id();
            auto param = Nodes::FunctionParam(type, name);
            fn->AddParam(param);
            currToken = m_lexer->GetToken()->GetType();
        }

        Eat(TokenType::CloseBracket);

        Eat(TokenType::Colon);

        // TODO: support for complex types
        std::shared_ptr<Node> retType = Id();

        fn->SetReturnType(retType);

        Eat(TokenType::OpenCurly);

        while (m_lexer->GetToken()->GetType() != TokenType::CloseCurly && m_lexer->GetToken()->GetType() != TokenType::_EOF)
        {
            std::shared_ptr<Node> statement = Statement();
            body->AddItem(statement);
        }

        Eat(TokenType::CloseCurly);

        return fn;
    }

    std::shared_ptr<Node> Parser::ReturnStatement()
    {
        auto token = m_lexer->GetToken();
        Eat(TokenType::Return);

        std::shared_ptr<Node> value = Ternary();

        return std::make_shared<Nodes::ReturnNode>(value, token->GetLocation());
    }

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

		std::shared_ptr<Nodes::ProcedureDeclaration> procedure =
			std::make_shared<Nodes::ProcedureDeclaration>(nameId->GetName(), body, procLoc);

		return procedure;
	}

	std::shared_ptr<Node> Parser::VariableDeclaration()
	{
        auto keyword = m_lexer->GetToken();
		bool editable = keyword->GetType() == TokenType::Var;
        auto varLoc = keyword->GetLocation();

		if (editable)
			Eat(TokenType::Var);
		else
			Eat(TokenType::Const);

        // TODO: support for complex types
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
			return std::make_shared<Nodes::VarDeclaration>(type, name, value, editable, varLoc);
		else
			return std::make_shared<Nodes::VarDeclaration>(type, name, varLoc);
	}

	std::shared_ptr<Node> Parser::IfElseStatement()
	{
		// if (5 == 5 ? true : false) {} else {}
        auto loc = m_lexer->GetToken()->GetLocation();
		Eat(TokenType::If);
		// Eat(TokenType::OpenBracket)
		std::shared_ptr<Node> condition = Ternary();
		// Eat(TokenType::CloseBracket)

		auto ifElse = std::make_shared<Statements::IfElseStatement>(condition, loc);

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
				auto stmts = std::make_shared<Statements::StatementsBody>(m_lexer->GetToken()->GetLocation());
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
