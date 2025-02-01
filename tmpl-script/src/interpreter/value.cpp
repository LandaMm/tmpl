#include <cassert>
#include <memory>
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

    // List Value

    std::string Runtime::ListValue::format() const
    {
        std::string res = "List[";
        for (size_t i = 0; i < m_value.size(); i++)
        {
            res += m_value[i]->format();
            if (i + 1 < m_value.size())
            {
                res += ", ";
            }
        }
        return res + "]";
    }

    std::shared_ptr<Value> Runtime::ListValue::Compare(std::shared_ptr<Value> right,
            AST::Nodes::Condition::ConditionType condition)
    {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Lists are not compareable");
			return nullptr;
    }

    std::shared_ptr<Value> Runtime::ListValue::Operate(std::shared_ptr<Value> right,
            AST::Nodes::ExpressionNode::OperatorType opType)
    {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Lists are not operateable");
			return nullptr;
    }

    void Runtime::ListValue::AddItem(std::shared_ptr<Value> item)
    {
        m_value.push_back(item);
    }

    std::shared_ptr<Value> Runtime::ListValue::GetItem(std::shared_ptr<Value> indexVal)
    {
        if (indexVal->GetType() != ValueType::Integer)
        {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Non-integers values cannot be used as index for a list");
			return nullptr;
        }

        std::shared_ptr<IntegerValue> vl = std::dynamic_pointer_cast<IntegerValue>(indexVal);
        std::shared_ptr<int> index = vl->GetValue();

        if (*index < 0)
        {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Negative numbers cannot be used as index for list");
			return nullptr;
        }

        if (m_value.size() <= *index)
        {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Index is out of range of the list");
			return nullptr;
        }

        return m_value[*index];
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

        assert(condition != AST::Nodes::Condition::ConditionType::None &&
                "Exhausted compare operator handlers for integers");

        assert(false && "Unreachable code. Integer value compare");

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

        assert(opType != AST::Nodes::ExpressionNode::OperatorType::NONE &&
                "Exhausted operate operation handlers for integer");

        assert(false && "Unreachable code. Integer value operate");

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

        assert(condition != AST::Nodes::Condition::ConditionType::None &&
                "Exhausted compare operator handlers for float");

        assert(false && "Unreachable code. Float value compare");

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

        assert(opType != AST::Nodes::ExpressionNode::OperatorType::NONE &&
                "Exhausted operate operation handlers for float");

        assert(false && "Unreachable code. Float value operate");

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

        assert(condition != AST::Nodes::Condition::ConditionType::None &&
                "Exhausted compare operator handlers for double");

        assert(false && "Unreachable code. Double value compare");

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

        assert(opType != AST::Nodes::ExpressionNode::OperatorType::NONE &&
                "Exhausted operate operation handlers for doubles");

        assert(false && "Unreachable code. Double value operate");

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

        assert(false && "Unreachable code. String value compare");

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

        assert(false && "Unreachable string operator condition met.");

		return nullptr;
	}
}
