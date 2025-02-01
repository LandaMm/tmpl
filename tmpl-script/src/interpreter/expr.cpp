
#include "../../include/interpreter.h"

namespace Runtime
{
    using namespace AST::Nodes;

	std::shared_ptr<Value> Interpreter::EvaluateTernary(std::shared_ptr<TernaryNode> ternary)
	{
		std::shared_ptr<Value> condition = Execute(ternary->GetCondition());

		if (condition->GetType() != ValueType::Integer)
		{
			// should be unreachable
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("RuntimeError: Condition cannot produce non-integer value");
			return nullptr;
		}

		std::shared_ptr<IntegerValue> cVal = std::dynamic_pointer_cast<IntegerValue>(condition);
		if (*cVal->GetValue() == 1)
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
			errorManager.OperandMismatchType(left->GetType(), right->GetType());
			return nullptr;
		}

		if (left->GetType() != right->GetType() && (left->GetType() == ValueType::Null || right->GetType() == ValueType::Null))
		{
			if (condition->GetOperator() != Condition::ConditionType::Compare && condition->GetOperator() != Condition::ConditionType::NotEqual)
			{
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.RaiseError("Unsupported condition operator for null values: " + std::to_string((int)condition->GetOperator()));
				return nullptr;
			}
			if (condition->GetOperator() == Condition::ConditionType::Compare)
				return std::make_shared<IntegerValue>(std::make_shared<int>(0));
			else
				return std::make_shared<IntegerValue>(std::make_shared<int>(1));
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
			errorManager.RaiseError("Unsupported different types of operands: " + std::to_string((int)left->GetType()) + " != " + std::to_string((int)right->GetType()));
			return nullptr;
		}

		return left->Operate(right, expr->GetOperator().GetType());
	}
}

