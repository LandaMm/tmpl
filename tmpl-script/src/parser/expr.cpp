
#include "../../include/parser.h"
#include "../../include/node/expression.h"
#include "../../include/node/literal.h"
#include "../../include/node/logical.h"
#include "../../include/node/unary.h"
#include "include/node/assign.h"
#include "include/node/instance.h"
#include "include/token.h"

namespace AST
{
    std::shared_ptr<Node> Parser::Assignment()
    {
        // TODO: support for object member assignment
        if (m_lexer->GetToken()->GetType() == TokenType::Id)
        {
            // TODO: when support object members use m_lexer.SaveState to look for "="
            if (m_lexer->SeekToken() != nullptr &&
                    (
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
                    case TokenType::Equal:
                        assignOp = Nodes::AssignOperator::Reassign;
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

                auto expr = Ternary();

                return std::make_shared<Nodes::AssignmentNode>(assignee, expr, assignOp, assignee->GetLocation());
            }
        }

        return Ternary();
    }

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

			auto node = std::make_shared<Nodes::TernaryNode>(result->GetLocation());

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
		std::shared_ptr<Nodes::Condition> expr = std::make_shared<Nodes::Condition>(result->GetLocation());
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
			expr = std::make_shared<Nodes::Condition>(result->GetLocation());
			expr->SetLeft(result);
		}

		return result;
	}

	std::shared_ptr<Node> Parser::Expr()
	{
		std::shared_ptr<Node> result = Term();
		std::shared_ptr<Nodes::ExpressionNode> expr = std::make_shared<Nodes::ExpressionNode>(result->GetLocation());
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
				expr = std::make_shared<Nodes::ExpressionNode>(result->GetLocation());
				expr->SetLeft(result);
			}
			else if (token->GetType() == TokenType::Minus)
			{
				Eat(TokenType::Minus);
				std::shared_ptr<Node> right = Term();
				expr->SetRight(right);
				expr->SetOperator(Nodes::ExpressionNode::Operator(Nodes::ExpressionNode::OperatorType::MINUS));

				result = expr;
				expr = std::make_shared<Nodes::ExpressionNode>(result->GetLocation());
				expr->SetLeft(result);
			}
		}

		return result;
	}

	std::shared_ptr<Node> Parser::Term()
	{
		std::shared_ptr<Node> result = Factor();
		std::shared_ptr<Nodes::ExpressionNode> expr =
            std::make_shared<Nodes::ExpressionNode>(Nodes::ExpressionNode(result->GetLocation()));
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
				expr = std::make_shared<Nodes::ExpressionNode>(result->GetLocation());
				expr->SetLeft(result);
			}
			else if (token->GetType() == TokenType::Divide)
			{
				Eat(TokenType::Divide);
				std::shared_ptr<Node> right = Factor();
				expr->SetRight(right);
				expr->SetOperator(Nodes::ExpressionNode::Operator(Nodes::ExpressionNode::OperatorType::DIVIDE));

				result = expr;
				expr = std::make_shared<Nodes::ExpressionNode>(result->GetLocation());
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
			std::shared_ptr<int> value = token->GetValue<int>();
			using Holder = Nodes::LiteralNode::TypedValueHolder<int>;
			std::shared_ptr<Holder> v =
				std::make_shared<Holder>(std::make_shared<int>(*value));
			return std::make_shared<Nodes::LiteralNode>(Nodes::LiteralType::INT, v, token->GetLocation());
		}
		// float literal
		else if (token->GetType() == TokenType::Float)
		{
			Eat(TokenType::Float);
			std::shared_ptr<float> value = token->GetValue<float>();
			using Holder = Nodes::LiteralNode::TypedValueHolder<float>;
			std::shared_ptr<Holder> v =
				std::make_shared<Holder>(std::make_shared<float>(*value));
			return std::make_shared<Nodes::LiteralNode>(Nodes::LiteralType::FLOAT, v, token->GetLocation());
		}
		// double literal
		else if (token->GetType() == TokenType::Double)
		{
			Eat(TokenType::Double);
			std::shared_ptr<double> value = token->GetValue<double>();
			using Holder = Nodes::LiteralNode::TypedValueHolder<double>;
			std::shared_ptr<Holder> v =
				std::make_shared<Holder>(std::make_shared<double>(*value));
			return std::make_shared<Nodes::LiteralNode>(Nodes::LiteralType::DOUBLE, v, token->GetLocation());
		}
		// string literal
		else if (token->GetType() == TokenType::String)
		{
			Eat(TokenType::String);
			std::shared_ptr<std::string> value = token->GetValue<std::string>();
			using Holder = Nodes::LiteralNode::TypedValueHolder<std::string>;
			std::shared_ptr<Holder> v =
				std::make_shared<Holder>(std::make_shared<std::string>(*value));
			return std::make_shared<Nodes::LiteralNode>(Nodes::LiteralType::STRING, v, token->GetLocation());
		}
		else if (token->GetType() == TokenType::True || token->GetType() == TokenType::False)
        {
            bool active = token->GetType() == TokenType::True;
			Eat(active ? TokenType::True : TokenType::False);
			using Holder = Nodes::LiteralNode::TypedValueHolder<bool>;
			std::shared_ptr<Holder> v =
				std::make_shared<Holder>(std::make_shared<bool>(active));
			return std::make_shared<Nodes::LiteralNode>(Nodes::LiteralType::BOOL, v, token->GetLocation());
        }
        /*else if (token->GetType() == TokenType::Boo)*/
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
			return std::make_shared<Nodes::UnaryNode>(Nodes::UnaryNode::UnaryOperator::Not, Factor(), token->GetLocation());
		}
		// +factor
		else if (token->GetType() == TokenType::Plus)
		{
			Eat(TokenType::Plus);
			return std::make_shared<Nodes::UnaryNode>(Nodes::UnaryNode::UnaryOperator::Positive, Factor(), token->GetLocation());
		}
		// -factor
		else if (token->GetType() == TokenType::Minus)
		{
			Eat(TokenType::Minus);
			return std::make_shared<Nodes::UnaryNode>(Nodes::UnaryNode::UnaryOperator::Negative, Factor(), token->GetLocation());
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

            std::shared_ptr<Node> res = Ternary();
            Eat(TokenType::CloseBracket);

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
		else if (token->GetType() == TokenType::New)
        {
            auto loc = m_lexer->GetToken()->GetLocation();
            Eat(TokenType::New);
            auto fnName = Type();
            auto fCall = FunctionCall(fnName);
            return std::make_shared<Nodes::InstanceNode>(fnName, fCall, loc);
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
