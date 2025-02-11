
#include "../../include/typechecker.h"

namespace Runtime
{
    using namespace AST::Nodes;

    std::shared_ptr<ComplexValueType> TypeChecker::DiagnoseExpression(std::shared_ptr<ExpressionNode> expr)
    {
        auto left = DiagnoseNode(expr->GetLeft());
        auto right = DiagnoseNode(expr->GetRight());

        if (left != right)
        {
            Prelude::ErrorManager& manager = Prelude::ErrorManager::getInstance();
            manager.OperandMismatchType(GetFilename(), left, right, expr->GetLocation(), "TypeError");
            ReportError();
            // TODO: maybe there is some better
            // e.g. add typechecker's only value type "Mixed"
            // that passes all type tests?
            return left;
        }

        return left;
    }

    std::shared_ptr<ComplexValueType> TypeChecker::DiagnoseCondition(std::shared_ptr<Condition> condition)
    {
		std::shared_ptr<ComplexValueType> left = DiagnoseNode(condition->GetLeft());
		std::shared_ptr<ComplexValueType> right = DiagnoseNode(condition->GetRight());

        auto nullVal = std::make_shared<ComplexValueType>(ValueType::Null);

		if (left != right && !left->Compare(nullVal) && !right->Compare(nullVal))
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.OperandMismatchType(GetFilename(), left, right, condition->GetLocation(), "RuntimeError");
            ReportError();
			return std::make_shared<ComplexValueType>(ValueType::Bool);
		}

		if (left != right && (left->Compare(nullVal) || right->Compare(nullVal)))
		{
			if (condition->GetOperator() != Condition::ConditionType::Compare && condition->GetOperator() != Condition::ConditionType::NotEqual)
			{
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.RaiseError("Unsupported condition operator for null values: " + std::to_string((int)condition->GetOperator()), "TypeError");
                ReportError();
				return std::make_shared<ComplexValueType>(ValueType::Bool);
			}
            return std::make_shared<ComplexValueType>(ValueType::Bool);
		}

		return std::make_shared<ComplexValueType>(ValueType::Bool);
    }

    std::shared_ptr<ComplexValueType> TypeChecker::DiagnoseTernary(std::shared_ptr<TernaryNode> ternary)
    {
		std::shared_ptr<ComplexValueType> condition = DiagnoseNode(ternary->GetCondition());

        auto leftType = DiagnoseNode(ternary->GetLeft());
        auto rightType = DiagnoseNode(ternary->GetRight());

		if (!condition->Compare(std::make_shared<ComplexValueType>(ValueType::Bool)))
		{
			// should be unreachable
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("RuntimeError: Condition cannot produce non-boolean value", "TypeError");
                ReportError();
		    // TODO: mixed type (see DiagnoseExpression)
            return leftType;
		}

        if (!leftType->Compare(rightType)) {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.TypeMismatch(GetFilename(), leftType, rightType, ternary->GetLocation());
                ReportError();
        }

		return leftType;
    }

    std::shared_ptr<ComplexValueType> TypeChecker::DiagnoseUnary(std::shared_ptr<UnaryNode> unary)
    {
        auto op = unary->GetOperator();
        if (op == UnaryNode::UnaryOperator::Negative || op == UnaryNode::UnaryOperator::Positive)
        {
            return DiagnoseNode(unary->GetTarget());
        }

        return std::make_shared<ComplexValueType>(ValueType::Bool);
    }
}

