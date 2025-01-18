
#include"../../include/parser.h"
#include"../../include/node/expression.h"
#include"../../include/node/identifier.h"
#include"../../include/node/literal.h"
#include"../../include/node/function.h"
#include"../../include/node/logical.h"
#include"../../include/node/unary.h"

namespace Compiler
{
	std::shared_ptr<Node> Parser::Ternary()
	{
		std::shared_ptr<Node> result = Cond();

		// 5 == 5 ? 3 + 2 == 1 + 4 ? true : false : false
		if (m_lexer->GetToken()->GetType() == TokenType::Question)
		{
			Eat(TokenType::Question);

			std::shared_ptr<Node> left = Ternary();
			Eat(TokenType::Colon);
			std::shared_ptr<Node> right = Ternary();

			auto node = std::make_shared<Nodes::TernaryNode>();

			node->SetCondition(result);
			node->SetLeft(left);
			node->SetRight(right);

			return node;
		}

		return result;
	}

	std::shared_ptr<Node> Parser::Cond()
	{
		std::shared_ptr<Node> result = Expr();
		std::shared_ptr<Nodes::Condition> expr = std::make_shared<Nodes::Condition>();
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

			std::shared_ptr<Node> right = Expr();
			expr->SetRight(right);

			result = expr;
			expr = std::make_shared<Nodes::Condition>();
			expr->SetLeft(result);
		}

		return result;
	}

	std::shared_ptr<Node> Parser::Expr()
	{
		std::shared_ptr<Node> result = Term();
		std::shared_ptr<Nodes::ExpressionNode> expr = std::make_shared<Nodes::ExpressionNode>();
		expr->SetLeft(result);

		while (m_lexer->GetToken()->GetType() == TokenType::Plus || m_lexer->GetToken()->GetType() == TokenType::Minus)
		{
			auto token = m_lexer->GetToken();

			if (token->GetType() == TokenType::Plus)
			{
				Eat(TokenType::Plus);
				std::shared_ptr<Node> right = Term();
				expr->SetRight(right);
				expr->SetOperator(Nodes::ExpressionNode::Operator(Nodes::ExpressionNode::OperatorType::PLUS));

				result = expr;
				expr = std::make_shared<Nodes::ExpressionNode>();
				expr->SetLeft(result);
			}
			else if (token->GetType() == TokenType::Minus)
			{
				Eat(TokenType::Minus);
				std::shared_ptr<Node> right = Term();
				expr->SetRight(right);
				expr->SetOperator(Nodes::ExpressionNode::Operator(Nodes::ExpressionNode::OperatorType::MINUS));

				result = expr;
				expr = std::make_shared<Nodes::ExpressionNode>();
				expr->SetLeft(result);
			}
		}

		return result;
	}

	std::shared_ptr<Node> Parser::Term()
	{
		std::shared_ptr<Node> result = Factor();
		std::shared_ptr<Nodes::ExpressionNode> expr = std::make_shared<Nodes::ExpressionNode>(Nodes::ExpressionNode());
		expr->SetLeft(result);

		while (m_lexer->GetToken()->GetType() == TokenType::Multiply || m_lexer->GetToken()->GetType() == TokenType::Divide)
		{
			auto token = m_lexer->GetToken();

			if (token->GetType() == TokenType::Multiply)
			{
				Eat(TokenType::Multiply);
				std::shared_ptr<Node> right = Factor();
				expr->SetRight(right);
				expr->SetOperator(Nodes::ExpressionNode::Operator(Nodes::ExpressionNode::OperatorType::MULTIPLY));

				result = expr;
				expr = std::make_shared<Nodes::ExpressionNode>();
				expr->SetLeft(result);
			}
			else if (token->GetType() == TokenType::Divide)
			{
				Eat(TokenType::Divide);
				std::shared_ptr<Node> right = Factor();
				expr->SetRight(right);
				expr->SetOperator(Nodes::ExpressionNode::Operator(Nodes::ExpressionNode::OperatorType::DIVIDE));

				result = expr;
				expr = std::make_shared<Nodes::ExpressionNode>();
				expr->SetLeft(result);
			}
		}

		return result;
	}

	std::shared_ptr<Node> Parser::Factor()
	{
		// var|(expr)|number (+|-) var|(expr)|number
		// can also be function call: calc1() + calc(2)
		// and string: "some_string" + "concat"
		// object (+ function call): some.method.inside.class()
		// !var || -5 || +3 (unary)

		auto token = m_lexer->GetToken();

		// integer literal
		if (token->GetType() == TokenType::Integer)
		{
			Eat(TokenType::Integer);
			int* value = token->GetValue<int>();
			return std::make_shared<Nodes::LiteralNode>(Nodes::LiteralType::INT, value);
		}
		// float literal
		else if (token->GetType() == TokenType::Float)
		{
			Eat(TokenType::Float);
			float* value = token->GetValue<float>();
			return std::make_shared<Nodes::LiteralNode>(Nodes::LiteralType::FLOAT, value);
		}
		// double literal
		else if (token->GetType() == TokenType::Double)
		{
			Eat(TokenType::Float);
			double* value = token->GetValue<double>();
			return std::make_shared<Nodes::LiteralNode>(Nodes::LiteralType::DOUBLE, value);
		}
		// string literal
		else if (token->GetType() == TokenType::String)
		{
			Eat(TokenType::String);
			std::string* value = token->GetValue<std::string>();
			return std::make_shared<Nodes::LiteralNode>(Nodes::LiteralType::STRING, value);
		}
		// id || function call with args | object member | obj_member + call | call + obj_member
		else if (token->GetType() == TokenType::Id)
		{
			std::shared_ptr<Node> res = Id();
			TokenType current_type = m_lexer->GetToken()->GetType();
			while (current_type == TokenType::OpenBracket || current_type == TokenType::Point || current_type == TokenType::OpenSquareBracket)
			{
				if (current_type == TokenType::OpenBracket)
				{
					std::shared_ptr<Node> fcall = FunctionCall(res);
					res = fcall;
				}
				else if (current_type == TokenType::Point || current_type == TokenType::OpenSquareBracket)
				{
					std::shared_ptr<Node> objm = ObjectMember(res);
					res = objm;
				}
				current_type = m_lexer->GetToken()->GetType();
			}
			return res;
		}
		// [list]
		else if (token->GetType() == TokenType::OpenSquareBracket)
		{
			return List();
		}
		// !factor
		else if (token->GetType() == TokenType::Not)
		{
			Eat(TokenType::Not);
			return std::make_shared<Nodes::UnaryNode>(Nodes::UnaryNode::UnaryOperator::Not, Factor());
		}
		// +factor
		else if (token->GetType() == TokenType::Plus)
		{
			Eat(TokenType::Plus);
			return std::make_shared<Nodes::UnaryNode>(Nodes::UnaryNode::UnaryOperator::Positive, Factor());
		}
		// -factor
		else if (token->GetType() == TokenType::Minus)
		{
			Eat(TokenType::Minus);
			return std::make_shared<Nodes::UnaryNode>(Nodes::UnaryNode::UnaryOperator::Negative, Factor());
		}
		// (expr)
		else if (token->GetType() == TokenType::OpenBracket)
		{
			Eat(TokenType::OpenBracket);
			std::shared_ptr<Node> res = Ternary();
			Eat(TokenType::CloseBracket);
			return res;
		}
		// unknown
		else
		{
			Error("Unexpected token for factor met: " + std::to_string((int)token->GetType()));
			return nullptr;
		}
	}
}