#include <cassert>
#include <memory>
#include "../../include/interpreter/value.h"
#include "../../include/error.h"

namespace Runtime
{
	// Common
    
    std::ostream &operator<<(std::ostream& stream, const CustomValueType &x)
    {
        stream << x.m_name;

        return stream;
    }

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
			errorManager.RaiseError("Lists are not compareable", "RuntimeError");
			return nullptr;
    }

    std::shared_ptr<Value> Runtime::ListValue::Operate(std::shared_ptr<Value> right,
            AST::Nodes::ExpressionNode::OperatorType opType)
    {
        Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
        errorManager.RaiseError("Lists are not operateable", "RuntimeError");
        return nullptr;
    }

    void Runtime::ListValue::AddItem(std::shared_ptr<Value> item)
    {
        m_value.push_back(item);
    }

    std::shared_ptr<Value> Runtime::ListValue::GetItem(std::shared_ptr<IntegerValue> indexVal)
    {
        std::shared_ptr<int> index = indexVal->GetValue();

        if (*index < 0)
        {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Negative numbers cannot be used as index for list", "RuntimeError");
			return nullptr;
        }

        if (m_value.size() <= *index)
        {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Index is out of range of the list", "RuntimeError");
			return nullptr;
        }

        return m_value[*index];
    }

	// Bool Value

	std::string BoolValue::format() const
	{
        if (m_value) {
            return "BoolValue(true)";
        }
        return "BoolValue(false)";
	}

	std::shared_ptr<Value> Runtime::BoolValue::Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition)
	{
		std::shared_ptr<BoolValue> iright = std::dynamic_pointer_cast<BoolValue>(right);
		bool vleft = GetValue();
		bool vright = iright->GetValue();

		switch (condition)
		{
		case AST::Nodes::Condition::ConditionType::Compare:
			return std::make_shared<BoolValue>(vleft == vright);
		case AST::Nodes::Condition::ConditionType::NotEqual:
			return std::make_shared<BoolValue>(vleft != vright);
        default:
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported operator for bool literals: " + std::to_string((int)condition), "RuntimeError");
			return nullptr;
		}

        assert(condition != AST::Nodes::Condition::ConditionType::None &&
                "Exhausted compare operator handlers for bool");

        assert(false && "Unreachable code. Bool value compare");

		return nullptr;
	}

	std::shared_ptr<Value> Runtime::BoolValue::Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType)
	{
        Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
        errorManager.RaiseError("Bool values do not support any operations", "RuntimeError");

        assert(false && "Unreachable code. Bool value operate");

		return nullptr;
	}

    // VoidValue

	std::string VoidValue::format() const
	{
		return "VoidValue()";
	}

	std::shared_ptr<Value> Runtime::VoidValue::Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition)
	{
		std::shared_ptr<Value> iright = std::dynamic_pointer_cast<Value>(right);

		switch (condition)
		{
		case AST::Nodes::Condition::ConditionType::Compare:
			return std::make_shared<BoolValue>(iright->GetType()->Compare(ValType("void")));
		case AST::Nodes::Condition::ConditionType::NotEqual:
			return std::make_shared<BoolValue>(!iright->GetType()->Compare(ValType("void")));
		default:
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported operator for undefined: " + std::to_string((int)condition), "RuntimeError");
			return nullptr;
		}

        assert(condition != AST::Nodes::Condition::ConditionType::None &&
                "Exhausted compare operator handlers for null value");

        assert(false && "Unreachable code. Null value compare");

		return nullptr;
	}

	std::shared_ptr<Value> Runtime::VoidValue::Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType)
	{
        Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
        errorManager.RaiseError("Null values does not support any operations", "RuntimeError");
        return nullptr;
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
			return std::make_shared<BoolValue>(vleft == vright);
		case AST::Nodes::Condition::ConditionType::Less:
			return std::make_shared<BoolValue>(vleft < vright);
		case AST::Nodes::Condition::ConditionType::Greater:
			return std::make_shared<BoolValue>(vleft > vright);
		case AST::Nodes::Condition::ConditionType::LessEqual:
			return std::make_shared<BoolValue>(vleft <= vright);
		case AST::Nodes::Condition::ConditionType::GreaterEqual:
			return std::make_shared<BoolValue>(vleft >= vright);
		case AST::Nodes::Condition::ConditionType::NotEqual:
			return std::make_shared<BoolValue>(vleft != vright);
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
                {
                    auto nv = std::dynamic_pointer_cast<IntegerValue>(iright->Clone());
                    nv->SetValue(vleft / vright);
                    return nv;
                }
            case AST::Nodes::ExpressionNode::OperatorType::MULTIPLY:
                {
                    auto nv = std::dynamic_pointer_cast<IntegerValue>(iright->Clone());
                    nv->SetValue(vleft * vright);
                    return nv;
                }
            case AST::Nodes::ExpressionNode::OperatorType::PLUS:
                {
                    auto nv = std::dynamic_pointer_cast<IntegerValue>(iright->Clone());
                    nv->SetValue(vleft + vright);
                    return nv;
                }
            case AST::Nodes::ExpressionNode::OperatorType::MINUS:
                {
                    auto nv = std::dynamic_pointer_cast<IntegerValue>(iright->Clone());
                    nv->SetValue(vleft - vright);
                    return nv;
                }
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
			return std::make_shared<BoolValue>(vleft == vright);
		case AST::Nodes::Condition::ConditionType::Less:
			return std::make_shared<BoolValue>(vleft < vright);
		case AST::Nodes::Condition::ConditionType::Greater:
			return std::make_shared<BoolValue>(vleft > vright);
		case AST::Nodes::Condition::ConditionType::LessEqual:
			return std::make_shared<BoolValue>(vleft <= vright);
		case AST::Nodes::Condition::ConditionType::GreaterEqual:
			return std::make_shared<BoolValue>(vleft >= vright);
		case AST::Nodes::Condition::ConditionType::NotEqual:
			return std::make_shared<BoolValue>(vleft != vright);
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
                {
                    auto nv = std::dynamic_pointer_cast<FloatValue>(iright->Clone());
                    nv->SetValue(vleft / vright);
                    return nv;
                }
            case AST::Nodes::ExpressionNode::OperatorType::MULTIPLY:
                {
                    auto nv = std::dynamic_pointer_cast<FloatValue>(iright->Clone());
                    nv->SetValue(vleft * vright);
                    return nv;
                }
            case AST::Nodes::ExpressionNode::OperatorType::PLUS:
                {
                    auto nv = std::dynamic_pointer_cast<FloatValue>(iright->Clone());
                    nv->SetValue(vleft + vright);
                    return nv;
                }
            case AST::Nodes::ExpressionNode::OperatorType::MINUS:
                {
                    auto nv = std::dynamic_pointer_cast<FloatValue>(iright->Clone());
                    nv->SetValue(vleft - vright);
                    return nv;
                }
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
			return std::make_shared<BoolValue>(vleft == vright);
		case AST::Nodes::Condition::ConditionType::Less:
			return std::make_shared<BoolValue>(vleft < vright);
		case AST::Nodes::Condition::ConditionType::Greater:
			return std::make_shared<BoolValue>(vleft > vright);
		case AST::Nodes::Condition::ConditionType::LessEqual:
			return std::make_shared<BoolValue>(vleft <= vright);
		case AST::Nodes::Condition::ConditionType::GreaterEqual:
			return std::make_shared<BoolValue>(vleft >= vright);
		case AST::Nodes::Condition::ConditionType::NotEqual:
			return std::make_shared<BoolValue>(vleft != vright);
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
        {
            auto nv = std::dynamic_pointer_cast<DoubleValue>(iright->Clone());
            nv->SetValue(vleft / vright);
            return nv;
        }
		case AST::Nodes::ExpressionNode::OperatorType::MULTIPLY:
        {
            auto nv = std::dynamic_pointer_cast<DoubleValue>(iright->Clone());
            nv->SetValue(vleft * vright);
            return nv;
        }
		case AST::Nodes::ExpressionNode::OperatorType::PLUS:
        {
            auto nv = std::dynamic_pointer_cast<DoubleValue>(iright->Clone());
            nv->SetValue(vleft + vright);
            return nv;
        }
		case AST::Nodes::ExpressionNode::OperatorType::MINUS:
        {
            auto nv = std::dynamic_pointer_cast<DoubleValue>(iright->Clone());
            nv->SetValue(vleft - vright);
            return nv;
        }
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
			return std::make_shared<BoolValue>(vleft == vright);
		case AST::Nodes::Condition::ConditionType::NotEqual:
			return std::make_shared<BoolValue>(vleft != vright);
		default:
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported operator for string literals: " + std::to_string((int)condition), "RuntimeError");
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
        {
            auto nv = std::dynamic_pointer_cast<StringValue>(iright->Clone());
            nv->SetValue(vleft + vright);
            return nv;
        }
		default:
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported operator for string literals: " + std::to_string((int)opType), "RuntimeError");
			return nullptr;
		}

        assert(false && "Unreachable string operator condition met.");

		return nullptr;
	}
}
