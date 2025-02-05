
#include "../../include/typechecker.h"

namespace Runtime
{
    using namespace AST::Nodes;

    ValueType TypeChecker::DiagnoseExpression(std::shared_ptr<ExpressionNode> expr)
    {
        auto left = DiagnoseNode(expr->GetLeft());
        auto right = DiagnoseNode(expr->GetRight());

        if (left != right)
        {
            Prelude::ErrorManager& manager = Prelude::ErrorManager::getInstance();
            manager.OperandMismatchType(GetFilename(), left, right, expr->GetLocation(), "TypeError");
            ReportError();
            return ValueType::Null;
        }

        return left;
    }

    ValueType TypeChecker::DiagnoseCondition(std::shared_ptr<Condition> condition)
    {
		ValueType left = DiagnoseNode(condition->GetLeft());
		ValueType right = DiagnoseNode(condition->GetRight());

		if (left != right && left != ValueType::Null && right != ValueType::Null)
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.OperandMismatchType(GetFilename(), left, right, condition->GetLocation(), "RuntimeError");
            ReportError();
			return ValueType::Null;
		}

		if (left != right && (left == ValueType::Null || right == ValueType::Null))
		{
			if (condition->GetOperator() != Condition::ConditionType::Compare && condition->GetOperator() != Condition::ConditionType::NotEqual)
			{
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.RaiseError("Unsupported condition operator for null values: " + std::to_string((int)condition->GetOperator()), "TypeError");
                ReportError();
				return ValueType::Null;
			}
            return ValueType::Bool;
		}

		return ValueType::Bool;
    }

    ValueType TypeChecker::DiagnoseTernary(std::shared_ptr<TernaryNode> ternary)
    {
		ValueType condition = DiagnoseNode(ternary->GetCondition());

        // TODO: boolean support
		if (condition != ValueType::Bool)
		{
			// should be unreachable
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("RuntimeError: Condition cannot produce non-boolean value", "TypeError");
                ReportError();
			return ValueType::Null;
		}

        auto leftType = DiagnoseNode(ternary->GetLeft());
        auto rightType = DiagnoseNode(ternary->GetRight());

        if (leftType != rightType) {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.TypeMismatch(GetFilename(), leftType, rightType, ternary->GetLocation());
                ReportError();
        }

		return leftType;
    }

    ValueType TypeChecker::DiagnoseUnary(std::shared_ptr<UnaryNode> unary)
    {
        auto op = unary->GetOperator();
        if (op == UnaryNode::UnaryOperator::Negative || op == UnaryNode::UnaryOperator::Positive)
        {
            return DiagnoseNode(unary->GetTarget());
        }

        return ValueType::Bool;
    }
}

