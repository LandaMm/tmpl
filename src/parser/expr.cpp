
#include "parser.h"
#include "node/expression.hpp"
#include "node/literal.hpp"
#include "node/logical.hpp"
#include "node/unary.hpp"
#include "node/assign.hpp"
#include "token.h"

namespace AST
{
    Node* Parser::Assignment()
    {
        if (m_lexer->GetToken()->GetType() == TokenType::Id)
        {
            if (m_lexer->SeekToken() != nullptr &&
                    (
                     m_lexer->SeekToken()->GetType() == TokenType::ColonEqual ||
                     m_lexer->SeekToken()->GetType() == TokenType::Equal ||
                     m_lexer->SeekToken()->GetType() == TokenType::CompoundAdd ||
                     m_lexer->SeekToken()->GetType() == TokenType::CompoundMinus ||
                     m_lexer->SeekToken()->GetType() == TokenType::CompoundMultiply ||
                     m_lexer->SeekToken()->GetType() == TokenType::CompoundDivide
                    ))
            {
                auto assignee = Id();

                Nodes::AssignOperator assignOp;

                switch (m_lexer->GetToken()->GetType())
                {
					case TokenType::ColonEqual:
						assignOp = Nodes::AssignOperator::Declare;
						break;
                    case TokenType::Equal:
                        assignOp = Nodes::AssignOperator::Assign;
                        break;
                    case TokenType::CompoundAdd:
                        assignOp = Nodes::AssignOperator::Add;
                        break;
                    case TokenType::CompoundMinus:
                        assignOp = Nodes::AssignOperator::Subtract;
                        break;
                    case TokenType::CompoundMultiply:
                        assignOp = Nodes::AssignOperator::Multiply;
                        break;
                    case TokenType::CompoundDivide:
                        assignOp = Nodes::AssignOperator::Divide;
                        break;
                    default:
                    {
                        Prelude::ErrorManager& errManager = GetErrorManager();
                        errManager.UnexpectedToken(GetFilename(), m_lexer->GetToken(), "assign operator (e.g. '=', '+=')");
                        return nullptr;
                    }
                }

                Eat(m_lexer->GetToken()->GetType());

				// By calling Assignment again,
				// we allow something like that:
				// a := 5;
				// x := y := a += 5;
				// Expected result: x = y = a = 10
                auto expr = Assignment();

                return m_arena.Alloc<Nodes::AssignmentNode>(assignee, expr, assignOp, assignee->GetLocation());
            }
        }

        return Ternary();
    }

	Node* Parser::Ternary()
	{
		Node* result = Cond();

		// 5 == 5 ? 3 + 2 == 1 + 4 ? true : false : false
		if (m_lexer->GetToken()->GetType() == TokenType::Question)
		{
			Eat(TokenType::Question);

			Node* left = Ternary();
			Eat(TokenType::Colon);
			Node* right = Ternary();

			auto node = m_arena.Alloc<Nodes::TernaryNode>(result->GetLocation());

			node->SetCondition(result);
			node->SetLeft(left);
			node->SetRight(right);

			return node;
		}

		return result;
	}

	Node* Parser::Cond()
	{
		Node* result = Expr();
		Nodes::Condition* expr = m_arena.Alloc<Nodes::Condition>(result->GetLocation());
		expr->SetLeft(result);

		while (m_lexer->GetToken()->GetType() == TokenType::Less || m_lexer->GetToken()->GetType() == TokenType::Greater ||
			   m_lexer->GetToken()->GetType() == TokenType::LessEqual || m_lexer->GetToken()->GetType() == TokenType::GreaterEqual ||
			   m_lexer->GetToken()->GetType() == TokenType::Compare || m_lexer->GetToken()->GetType() == TokenType::NotEqual)
		{
			auto token = m_lexer->GetToken();
			if (token->GetType() == TokenType::Greater)
			{
				Eat(TokenType::Greater);
				expr->SetOp(Nodes::Condition::ConditionType::Greater);
			}
			else if (token->GetType() == TokenType::Less)
			{
				Eat(TokenType::Less);
				expr->SetOp(Nodes::Condition::ConditionType::Less);
			}
			else if (token->GetType() == TokenType::LessEqual)
			{
				Eat(TokenType::LessEqual);
				expr->SetOp(Nodes::Condition::ConditionType::LessEqual);
			}
			else if (token->GetType() == TokenType::GreaterEqual)
			{
				Eat(TokenType::GreaterEqual);
				expr->SetOp(Nodes::Condition::ConditionType::GreaterEqual);
			}
			else if (token->GetType() == TokenType::Compare)
			{
				Eat(TokenType::Compare);
				expr->SetOp(Nodes::Condition::ConditionType::Compare);
			}
			else if (token->GetType() == TokenType::NotEqual)
			{
				Eat(TokenType::NotEqual);
				expr->SetOp(Nodes::Condition::ConditionType::NotEqual);
			}

			Node* right = Expr();
			expr->SetRight(right);

			result = expr;
			expr = m_arena.Alloc<Nodes::Condition>(result->GetLocation());
			expr->SetLeft(result);
		}

		return result;
	}

	Node* Parser::Expr()
	{
		Node* result = Term();
		Nodes::ExpressionNode* expr = m_arena.Alloc<Nodes::ExpressionNode>(result->GetLocation());
		expr->SetLeft(result);

		while (m_lexer->GetToken()->GetType() == TokenType::Plus || m_lexer->GetToken()->GetType() == TokenType::Minus)
		{
			auto token = m_lexer->GetToken();

			if (token->GetType() == TokenType::Plus)
			{
				Eat(TokenType::Plus);
				Node* right = Term();
				expr->SetRight(right);
				expr->SetOperator(Operator(OperatorType::PLUS));

				result = expr;
				expr = m_arena.Alloc<Nodes::ExpressionNode>(result->GetLocation());
				expr->SetLeft(result);
			}
			else if (token->GetType() == TokenType::Minus)
			{
				Eat(TokenType::Minus);
				Node* right = Term();
				expr->SetRight(right);
				expr->SetOperator(Operator(OperatorType::MINUS));

				result = expr;
				expr = m_arena.Alloc<Nodes::ExpressionNode>(result->GetLocation());
				expr->SetLeft(result);
			}
		}

		return result;
	}

	Node* Parser::Term()
	{
		Node* result = Factor();
		Nodes::ExpressionNode* expr =
            m_arena.Alloc<Nodes::ExpressionNode>(result->GetLocation());
		expr->SetLeft(result);

		while (m_lexer->GetToken()->GetType() == TokenType::Multiply || m_lexer->GetToken()->GetType() == TokenType::Divide)
		{
			auto token = m_lexer->GetToken();

			if (token->GetType() == TokenType::Multiply)
			{
				Eat(TokenType::Multiply);
				Node* right = Factor();
				expr->SetRight(right);
				expr->SetOperator(Operator(OperatorType::MULTIPLY));

				result = expr;
				expr = m_arena.Alloc<Nodes::ExpressionNode>(result->GetLocation());
				expr->SetLeft(result);
			}
			else if (token->GetType() == TokenType::Divide)
			{
				Eat(TokenType::Divide);
				Node* right = Factor();
				expr->SetRight(right);
				expr->SetOperator(Operator(OperatorType::DIVIDE));

				result = expr;
				expr = m_arena.Alloc<Nodes::ExpressionNode>(result->GetLocation());
				expr->SetLeft(result);
			}
		}

		return result;
	}

	Node* Parser::Factor()
	{
		// var|(expr)|number (+|-) var|(expr)|number
		// can also be function call: calc1() + calc(2)
		// and string: "some_string" + "concat"
		// !var || -5 || +3 (unary)

		auto token = m_lexer->GetToken();

		// integer literal
		if (token->GetType() == TokenType::Integer)
		{
			Eat(TokenType::Integer);
			int* value = token->GetValue<int>().get();
			using Holder = Nodes::TypedValueHolder<int>;
			Holder* v =
				m_arena.Alloc<Holder>(m_arena.Alloc<int>(*value));
			return m_arena.Alloc<Nodes::LiteralNode>(Nodes::LiteralType::INT, v, token->GetLocation());
		}
		// float literal
		else if (token->GetType() == TokenType::Float)
		{
			Eat(TokenType::Float);
			float* value = token->GetValue<float>().get();
			using Holder = Nodes::TypedValueHolder<float>;
			Holder* v =
				m_arena.Alloc<Holder>(m_arena.Alloc<float>(*value));
			return m_arena.Alloc<Nodes::LiteralNode>(Nodes::LiteralType::FLOAT, v, token->GetLocation());
		}
		// double literal
		else if (token->GetType() == TokenType::Double)
		{
			Eat(TokenType::Double);
			double* value = token->GetValue<double>().get();
			using Holder = Nodes::TypedValueHolder<double>;
			Holder* v =
				m_arena.Alloc<Holder>(m_arena.Alloc<double>(*value));
			return m_arena.Alloc<Nodes::LiteralNode>(Nodes::LiteralType::DOUBLE, v, token->GetLocation());
		}
		// string literal
		else if (token->GetType() == TokenType::String)
		{
			Eat(TokenType::String);
			std::string* value = token->GetValue<std::string>().get();
			using Holder = Nodes::TypedValueHolder<std::string>;
			Holder* v =
				m_arena.Alloc<Holder>(m_arena.Alloc<std::string>(*value));
			return m_arena.Alloc<Nodes::LiteralNode>(Nodes::LiteralType::STRING, v, token->GetLocation());
		}
		else if (token->GetType() == TokenType::True || token->GetType() == TokenType::False)
        {
            bool active = token->GetType() == TokenType::True;
			Eat(active ? TokenType::True : TokenType::False);
			using Holder = Nodes::TypedValueHolder<bool>;
			Holder* v =
				m_arena.Alloc<Holder>(m_arena.Alloc<bool>(active));
			return m_arena.Alloc<Nodes::LiteralNode>(Nodes::LiteralType::BOOL, v, token->GetLocation());
        }
		// id || function call with args
		else if (token->GetType() == TokenType::Id)
		{
			Node* res = Id();
			TokenType current_type = m_lexer->GetToken()->GetType();
			while (current_type == TokenType::OpenBracket || current_type == TokenType::Point || current_type == TokenType::OpenSquareBracket || current_type == TokenType::Less)
			{
                if (current_type == TokenType::OpenBracket)
                {
                    // normal function (without generics)
					Node* fcall = FunctionCall(res);
					res = fcall;
                }
				if (current_type == TokenType::Less)
				{
                    // f<box<int>, int>(...)
                    m_lexer->SaveState();
                    Eat(TokenType::Less);
                    if (m_lexer->GetToken()->GetType() != TokenType::Id)
                    {
                        m_lexer->RestoreState();
                        return res;
                    }
                    Eat(TokenType::Id);
                    if (!m_lexer->OneOf({TokenType::Less, TokenType::Comma, TokenType::Greater}, m_lexer->GetToken()->GetType()))
                    {
                        m_lexer->RestoreState();
                        return res;
                    }
                    m_lexer->RestoreState();

					Node* fcall = FunctionCall(res);
					res = fcall;
				}
				current_type = m_lexer->GetToken()->GetType();
			}
			return res;
		}
		// !factor
		else if (token->GetType() == TokenType::Not)
		{
			Eat(TokenType::Not);
			return m_arena.Alloc<Nodes::UnaryNode>(Nodes::UnaryOperator::Not, Factor(), token->GetLocation());
		}
		// +factor
		else if (token->GetType() == TokenType::Plus)
		{
			Eat(TokenType::Plus);
			return m_arena.Alloc<Nodes::UnaryNode>(Nodes::UnaryOperator::Positive, Factor(), token->GetLocation());
		}
		// -factor
		else if (token->GetType() == TokenType::Minus)
		{
			Eat(TokenType::Minus);
			return m_arena.Alloc<Nodes::UnaryNode>(Nodes::UnaryOperator::Negative, Factor(), token->GetLocation());
		}
		// (expr)
		else if (token->GetType() == TokenType::OpenBracket)
		{
			Eat(TokenType::OpenBracket);

            if (IsTypeCastAhead())
            {
                auto typ = Type();

                return Cast(typ);
            }

            Node* res = Ternary();
            Eat(TokenType::CloseBracket);

			TokenType current_type = m_lexer->GetToken()->GetType();
			while (current_type == TokenType::OpenBracket || current_type == TokenType::Point || current_type == TokenType::OpenSquareBracket || current_type == TokenType::Less)
			{
				if (current_type == TokenType::OpenBracket || current_type == TokenType::Less)
				{
					Node* fcall = FunctionCall(res);
					res = fcall;
				}
				current_type = m_lexer->GetToken()->GetType();
			}

            return res;
		}
		// unknown
		else
		{
			Prelude::ErrorManager &errManager = GetErrorManager();
			errManager.UnexpectedToken(m_lexer->GetFilename(), token);
			return nullptr;
		}
	}
}
