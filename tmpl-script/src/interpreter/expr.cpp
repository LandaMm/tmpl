
#include "../../include/interpreter.h"
#include <memory>
#include <cassert>

namespace Runtime
{
    using namespace AST::Nodes;

    std::shared_ptr<Value> Interpreter::EvaluateUnary(std::shared_ptr<UnaryNode> unary)
    {
        std::shared_ptr<Value> target = Execute(unary->GetTarget());

        if (unary->GetOperator() == UnaryNode::UnaryOperator::Positive)
        {
            return target;
        }

        if (unary->GetOperator() == UnaryNode::UnaryOperator::Negative)
        {
            switch(target->GetType())
            {
                case ValueType::Integer:
                {
                    std::shared_ptr<IntegerValue> val = std::dynamic_pointer_cast<IntegerValue>(target);
                    return std::make_shared<IntegerValue>(std::make_shared<int>(-(*val->GetValue())));
                }
                case ValueType::Float:
                {
                    std::shared_ptr<FloatValue> val = std::dynamic_pointer_cast<FloatValue>(target);
                    return std::make_shared<FloatValue>(std::make_shared<float>(-(*val->GetValue())));
                }
                case ValueType::Double:
                {
                    std::shared_ptr<DoubleValue> val = std::dynamic_pointer_cast<DoubleValue>(target);
                    return std::make_shared<DoubleValue>(std::make_shared<double>(-(*val->GetValue())));
                }
                default:
                    Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                    errorManager.UnaryOperatorNotSupported(GetFilename(), "-", target->GetType(), unary->GetLocation());
                    return nullptr;
            }
        }

        if (unary->GetOperator() == UnaryNode::UnaryOperator::Not)
        {
            // TODO: add boolean value support
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.UnaryOperatorNotSupported(GetFilename(), "!", target->GetType(), unary->GetLocation());
            return nullptr;
        }

        assert(false && "Unreachable code. Should have handled all unary operators");
        return nullptr;
    }

	std::shared_ptr<Value> Interpreter::EvaluateTernary(std::shared_ptr<TernaryNode> ternary)
	{
		std::shared_ptr<Value> condition = Execute(ternary->GetCondition());

        // TODO: boolean support
		if (condition->GetType() != ValueType::Bool)
		{
			// should be unreachable
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("RuntimeError: Condition cannot produce non-boolean value", "RuntimeError");
			return nullptr;
		}

		auto cVal = std::dynamic_pointer_cast<BoolValue>(condition);
		if (cVal->GetValue() == true)
		{
			return Execute(ternary->GetLeft());
		}

		return Execute(ternary->GetRight());
	}

	std::shared_ptr<Value> Interpreter::EvaluateCondition(std::shared_ptr<Condition> condition)
	{
		std::shared_ptr<Value> left = Execute(condition->GetLeft());
		std::shared_ptr<Value> right = Execute(condition->GetRight());

		if (left->GetType() != right->GetType() && left->GetType() != ValueType::Null && right->GetType() != ValueType::Null)
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.OperandMismatchType(GetFilename(), left->GetType(), right->GetType(), condition->GetLocation(), "RuntimeError");
			return nullptr;
		}

		if (left->GetType() != right->GetType() && (left->GetType() == ValueType::Null || right->GetType() == ValueType::Null))
		{
			if (condition->GetOperator() != Condition::ConditionType::Compare && condition->GetOperator() != Condition::ConditionType::NotEqual)
			{
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.RaiseError("Unsupported condition operator for null values: " + std::to_string((int)condition->GetOperator()), "RuntimeError");
				return nullptr;
			}
			if (condition->GetOperator() == Condition::ConditionType::Compare)
				return std::make_shared<BoolValue>(false);
			else
				return std::make_shared<BoolValue>(true);
		}

		return left->Compare(right, condition->GetOperator());
	}

	std::shared_ptr<Value> Interpreter::EvaluateExpression(std::shared_ptr<ExpressionNode> expr)
	{
		std::shared_ptr<Value> left = Execute(expr->GetLeft());
		std::shared_ptr<Value> right = Execute(expr->GetRight());

		ValueType valueType = left->GetType();
		if (valueType != right->GetType())
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported different types of operands: " + std::to_string((int)left->GetType()) + " != " + std::to_string((int)right->GetType()), "RuntimeError");
			return nullptr;
		}

		return left->Operate(right, expr->GetOperator().GetType());
	}
}

