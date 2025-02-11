#ifndef RUNTIME_VALUE_H
#define RUNTIME_VALUE_H
#include <memory>
#include <string>
#include <cassert>
#include <vector>
#include "../node/logical.h"
#include "../node/expression.h"

namespace Runtime
{
	enum class ValueType
	{
		Integer,
		Float,
		Double,
		String,
        List,
        Bool,
		Null,
	};

    struct ComplexValueType
    {
    private:
        ValueType m_base_type;
        std::shared_ptr<ComplexValueType> m_generic_type;

    public:
        ComplexValueType(ValueType baseType)
            : m_base_type(baseType), m_generic_type(nullptr) { }
        ComplexValueType(ValueType baseType, std::shared_ptr<ComplexValueType> genericType)
            : m_base_type(baseType), m_generic_type(genericType) { }
    public:
        void SetGenericType(std::shared_ptr<ComplexValueType> genericType)
        {
            m_generic_type = genericType;
        }
    public:
        inline ValueType GetBaseType() const { return m_base_type; }
        inline std::shared_ptr<ComplexValueType> GetGenericType() const { return m_generic_type; }
        inline bool HasGenericType() const { return m_generic_type != nullptr; }
    public:
        bool Compare(std::shared_ptr<ComplexValueType> other)
        {
            if (m_generic_type == nullptr && other->GetGenericType() == nullptr)
            {
                return m_base_type == other->GetBaseType();
            }
            else if (m_generic_type != nullptr && other->GetGenericType() != nullptr)
            {
                return (m_base_type == other->GetBaseType()) && m_generic_type->Compare(other->GetGenericType());
            }

            return false;
        }
    };

    static std::string HumanValueType(ValueType type)
    {
        switch(type)
        {
            case ValueType::String: return "string";
            case ValueType::Float: return "float";
            case ValueType::Integer: return "int";
            case ValueType::Double: return "double";
            case ValueType::Bool: return "bool";
            case ValueType::List: return "list";
            case ValueType::Null: return "null";
        }

        return "UNKNOWN_TYPE";
    }

    static std::string HumanValueType(std::shared_ptr<ComplexValueType> type)
    {
        std::string typeName = HumanValueType(type->GetBaseType());
        if (type->HasGenericType())
        {
            typeName += "<" + HumanValueType(type->GetGenericType()) + ">";
        }

        return typeName;
    }

	class Value
	{
	public:
		inline virtual std::shared_ptr<ComplexValueType> GetType() const = 0;

	public:
		template <typename T>
		inline T *Get() { return static_cast<T *>(this); }

	public:
		virtual std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) = 0;
		virtual std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) = 0;

	public:
		virtual ~Value() = default;

	public:
		virtual std::string format() const = 0;
	};

	std::ostream &operator<<(std::ostream &stream, const Value &value);
	std::ostream &operator<<(std::ostream &stream, const std::shared_ptr<Value> &value);


	class ListValue : public Value
	{
	private:
        std::shared_ptr<ComplexValueType> m_item_type;
		std::vector<std::shared_ptr<Value>> m_value;

	public:
		ListValue(std::shared_ptr<ComplexValueType> itemType)
            : m_value(std::vector<std::shared_ptr<Value>>()),
              m_item_type(itemType) {}
		ListValue(ValueType itemType)
            : m_value(std::vector<std::shared_ptr<Value>>()),
              m_item_type(std::make_shared<ComplexValueType>(itemType)) {}
		ListValue(std::shared_ptr<ComplexValueType> itemType, std::vector<std::shared_ptr<Value>> values)
            : m_value(values),
              m_item_type(itemType) { }
		ListValue(ValueType itemType, std::vector<std::shared_ptr<Value>> values)
            : m_value(values),
              m_item_type(std::make_shared<ComplexValueType>(itemType)) { }

	public:
		inline std::shared_ptr<ComplexValueType> GetType() const override
        {
            return std::make_shared<ComplexValueType>(ValueType::List, std::make_shared<ComplexValueType>(m_item_type));
        }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

    public:
        void AddItem(std::shared_ptr<Value> item);
        std::shared_ptr<Value> GetItem(std::shared_ptr<Value> indexVal);

	public:
		std::string format() const override;
	};

	class NullValue : public Value
	{
	public:
		NullValue() { }

	public:
		inline std::shared_ptr<ComplexValueType> GetType() const override
        {
            return std::make_shared<ComplexValueType>(ValueType::Null);
        }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

	public:
		std::string format() const override;
	};

	class BoolValue : public Value
	{
	private:
		bool m_value;

	public:
		BoolValue(bool value) : m_value(value) {}

	public:
		inline std::shared_ptr<ComplexValueType> GetType() const override
        {
            return std::make_shared<ComplexValueType>(ValueType::Bool);
        }

	public:
		inline bool GetValue() const { return m_value; }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

	public:
		std::string format() const override;
	};

	class IntegerValue : public Value
	{
	private:
		std::shared_ptr<int> m_value;

	public:
		IntegerValue(std::shared_ptr<int> value) : m_value(value) {}
		IntegerValue(int value) : m_value(std::make_shared<int>(value)) {}

	public:
		inline std::shared_ptr<ComplexValueType> GetType() const override
        {
            return std::make_shared<ComplexValueType>(ValueType::Integer);
        }

	public:
		inline std::shared_ptr<int> GetValue() const { return m_value; }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

	public:
		std::string format() const override;
	};
	class FloatValue : public Value
	{
	private:
		std::shared_ptr<float> m_value;

	public:
		FloatValue(std::shared_ptr<float> value) : m_value(value) {}
		FloatValue(float value) : m_value(std::make_shared<float>(value)) {}

	public:
		inline std::shared_ptr<ComplexValueType> GetType() const override
        {
            return std::make_shared<ComplexValueType>(ValueType::Float);
        }

	public:
		inline std::shared_ptr<float> GetValue() const { return m_value; }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

	public:
		std::string format() const override;
	};
	class DoubleValue : public Value
	{
	private:
		std::shared_ptr<double> m_value;

	public:
		DoubleValue(std::shared_ptr<double> value) : m_value(value) {}
		DoubleValue(double value) : m_value(std::make_shared<double>(value)) {}

	public:
		inline std::shared_ptr<ComplexValueType> GetType() const override
        {
            return std::make_shared<ComplexValueType>(ValueType::Double);
        }

	public:
		inline std::shared_ptr<double> GetValue() const { return m_value; }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

	public:
		std::string format() const override;
	};
	class StringValue : public Value
	{
	private:
		std::shared_ptr<std::string> m_value;

	public:
		StringValue(std::shared_ptr<std::string> value) : m_value(value) {}
		StringValue(std::string value) : m_value(std::make_shared<std::string>(value)) {}

	public:
		inline std::shared_ptr<ComplexValueType> GetType() const override
        {
            return std::make_shared<ComplexValueType>(ValueType::String);
        }

	public:
		inline std::shared_ptr<std::string> GetValue() const { return m_value; }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

	public:
		std::string format() const override;
	};
}

#endif
