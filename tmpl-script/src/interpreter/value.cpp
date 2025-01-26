#include "../../include/interpreter/value.h"
#include "../../include/error.h"

namespace Runtime
{
	// Common

	std::ostream &operator<<(std::ostream &stream, const Value &value)
	{
		stream << value.format();
		return stream;
	}

	std::ostream &operator<<(std::ostream &stream, const std::shared_ptr<Value> &value)
	{
		if (!value)
		{
			stream << "<EmptyValue>";
		}
		else
		{
			stream << value->format();
		}
		return stream;
	}

	// Integer Value

	std::string IntegerValue::format() const
	{
		return "IntegerValue(" + std::to_string(*m_value.get()) + ")";
	}

	std::shared_ptr<Value> Runtime::IntegerValue::Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition)
	{
		std::shared_ptr<IntegerValue> iright = std::dynamic_pointer_cast<IntegerValue>(right);
		int vleft = *GetValue();
		int vright = *iright->GetValue();

		switch (condition)
		{
		case AST::Nodes::Condition::ConditionType::Compare:
			return std::make_shared<IntegerValue>(vleft == vright);
		case AST::Nodes::Condition::ConditionType::Less:
			return std::make_shared<IntegerValue>(vleft < vright);
		case AST::Nodes::Condition::ConditionType::Greater:
			return std::make_shared<IntegerValue>(vleft > vright);
		case AST::Nodes::Condition::ConditionType::LessEqual:
			return std::make_shared<IntegerValue>(vleft <= vright);
		case AST::Nodes::Condition::ConditionType::GreaterEqual:
			return std::make_shared<IntegerValue>(vleft >= vright);
		case AST::Nodes::Condition::ConditionType::NotEqual:
			return std::make_shared<IntegerValue>(vleft != vright);
		}

		// TODO: raise error for unimplemented type

		return nullptr;
	}

	std::shared_ptr<Value> Runtime::IntegerValue::Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType)
	{
		std::shared_ptr<IntegerValue> iright = std::dynamic_pointer_cast<IntegerValue>(right);
		int vleft = *GetValue();
		int vright = *iright->GetValue();

		switch (opType)
		{
		case AST::Nodes::ExpressionNode::OperatorType::DIVIDE:
			return std::make_shared<IntegerValue>(vleft / vright);
		case AST::Nodes::ExpressionNode::OperatorType::MULTIPLY:
			return std::make_shared<IntegerValue>(vleft * vright);
		case AST::Nodes::ExpressionNode::OperatorType::PLUS:
			return std::make_shared<IntegerValue>(vleft + vright);
		case AST::Nodes::ExpressionNode::OperatorType::MINUS:
			return std::make_shared<IntegerValue>(vleft - vright);
		}

		// TODO: raise error for unimplemented type

		return nullptr;
	}

	// Float Value

	std::string FloatValue::format() const
	{
		return "FloatValue(" + std::to_string(*m_value) + ")";
	}

	std::shared_ptr<Value> Runtime::FloatValue::Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition)
	{
		std::shared_ptr<FloatValue> iright = std::dynamic_pointer_cast<FloatValue>(right);
		float vleft = *GetValue();
		float vright = *iright->GetValue();

		switch (condition)
		{
		case AST::Nodes::Condition::ConditionType::Compare:
			return std::make_shared<IntegerValue>(vleft == vright);
		case AST::Nodes::Condition::ConditionType::Less:
			return std::make_shared<IntegerValue>(vleft < vright);
		case AST::Nodes::Condition::ConditionType::Greater:
			return std::make_shared<IntegerValue>(vleft > vright);
		case AST::Nodes::Condition::ConditionType::LessEqual:
			return std::make_shared<IntegerValue>(vleft <= vright);
		case AST::Nodes::Condition::ConditionType::GreaterEqual:
			return std::make_shared<IntegerValue>(vleft >= vright);
		case AST::Nodes::Condition::ConditionType::NotEqual:
			return std::make_shared<IntegerValue>(vleft != vright);
		}

		// TODO: raise error for unimplemented type

		return nullptr;
	}

	std::shared_ptr<Value> Runtime::FloatValue::Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType)
	{
		std::shared_ptr<FloatValue> iright = std::dynamic_pointer_cast<FloatValue>(right);
		float vleft = *GetValue();
		float vright = *iright->GetValue();

		switch (opType)
		{
		case AST::Nodes::ExpressionNode::OperatorType::DIVIDE:
			return std::make_shared<FloatValue>(vleft / vright);
		case AST::Nodes::ExpressionNode::OperatorType::MULTIPLY:
			return std::make_shared<FloatValue>(vleft * vright);
		case AST::Nodes::ExpressionNode::OperatorType::PLUS:
			return std::make_shared<FloatValue>(vleft + vright);
		case AST::Nodes::ExpressionNode::OperatorType::MINUS:
			return std::make_shared<FloatValue>(vleft - vright);
		}

		// TODO: raise error for unimplemented type

		return nullptr;
	}

	// Double Value

	std::string DoubleValue::format() const
	{
		return "DoubleValue(" + std::to_string(*m_value) + ")";
	}

	std::shared_ptr<Value> Runtime::DoubleValue::Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition)
	{
		std::shared_ptr<DoubleValue> iright = std::dynamic_pointer_cast<DoubleValue>(right);
		double vleft = *GetValue();
		double vright = *iright->GetValue();

		switch (condition)
		{
		case AST::Nodes::Condition::ConditionType::Compare:
			return std::make_shared<IntegerValue>(vleft == vright);
		case AST::Nodes::Condition::ConditionType::Less:
			return std::make_shared<IntegerValue>(vleft < vright);
		case AST::Nodes::Condition::ConditionType::Greater:
			return std::make_shared<IntegerValue>(vleft > vright);
		case AST::Nodes::Condition::ConditionType::LessEqual:
			return std::make_shared<IntegerValue>(vleft <= vright);
		case AST::Nodes::Condition::ConditionType::GreaterEqual:
			return std::make_shared<IntegerValue>(vleft >= vright);
		case AST::Nodes::Condition::ConditionType::NotEqual:
			return std::make_shared<IntegerValue>(vleft != vright);
		}

		// TODO: raise error for unimplemented type

		return nullptr;
	}

	std::shared_ptr<Value> Runtime::DoubleValue::Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType)
	{
		std::shared_ptr<DoubleValue> iright = std::dynamic_pointer_cast<DoubleValue>(right);
		double vleft = *GetValue();
		double vright = *iright->GetValue();

		switch (opType)
		{
		case AST::Nodes::ExpressionNode::OperatorType::DIVIDE:
			return std::make_shared<DoubleValue>(vleft / vright);
		case AST::Nodes::ExpressionNode::OperatorType::MULTIPLY:
			return std::make_shared<DoubleValue>(vleft * vright);
		case AST::Nodes::ExpressionNode::OperatorType::PLUS:
			return std::make_shared<DoubleValue>(vleft + vright);
		case AST::Nodes::ExpressionNode::OperatorType::MINUS:
			return std::make_shared<DoubleValue>(vleft - vright);
		}

		// TODO: raise error for unimplemented type

		return nullptr;
	}

	// String Value

	std::string StringValue::format() const
	{
		return "StringValue(\"" + *m_value + "\")";
	}

	std::shared_ptr<Value> Runtime::StringValue::Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition)
	{
		std::shared_ptr<StringValue> iright = std::dynamic_pointer_cast<StringValue>(right);
		std::string vleft = *GetValue();
		std::string vright = *iright->GetValue();

		switch (condition)
		{
		case AST::Nodes::Condition::ConditionType::Compare:
			return std::make_shared<IntegerValue>(vleft == vright);
		case AST::Nodes::Condition::ConditionType::NotEqual:
			return std::make_shared<IntegerValue>(vleft != vright);
		default:
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported operator for string literals: " + std::to_string((int)condition));
			return nullptr;
		}

		// TODO: raise error for unimplemented type

		return nullptr;
	}

	std::shared_ptr<Value> Runtime::StringValue::Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType)
	{
		std::shared_ptr<StringValue> iright = std::dynamic_pointer_cast<StringValue>(right);
		std::string vleft = *GetValue();
		std::string vright = *iright->GetValue();

		switch (opType)
		{
		case AST::Nodes::ExpressionNode::OperatorType::PLUS:
			return std::make_shared<StringValue>(vleft + vright);
		default:
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported operator for string literals: " + std::to_string((int)opType));
			return nullptr;
		}

		// TODO: raise error for unimplemented type

		return nullptr;
	}
}