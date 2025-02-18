#ifndef RUNTIME_VALUE_H
#define RUNTIME_VALUE_H
#include <memory>
#include <ostream>
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

    struct CustomValueType
    {
    private:
        std::string m_name;

    public:
        CustomValueType(std::string name)
            : m_name(name) { }

    public:
        bool Compare(const CustomValueType& other) { return m_name == other.m_name; }

    public:
        inline std::string GetName() const { return m_name; }

    public:
        friend std::ostream &operator<<(std::ostream& stream, const CustomValueType &x);
    };

    typedef CustomValueType ValType;
    typedef std::shared_ptr<ValType> PValType;

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
            case ValueType::Null: return "void";
        }

        return "UNKNOWN_TYPE";
    }

	class Value
	{
	public:
		inline virtual PValType GetType() const = 0;

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
		std::vector<std::shared_ptr<Value>> m_value;

	public:
		ListValue() : m_value(std::vector<std::shared_ptr<Value>>()) {}
		ListValue(std::vector<std::shared_ptr<Value>> values)
            : m_value(values) {}

	public:
		inline PValType GetType() const override { return std::make_shared<ValType>("list"); }

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
		inline PValType GetType() const override { return std::make_shared<ValType>("void"); }

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
		inline PValType GetType() const override { return std::make_shared<ValType>("bool"); }

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
		inline PValType GetType() const override { return std::make_shared<ValType>("int"); }

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
		inline PValType GetType() const override { return std::make_shared<ValType>("float"); }

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
		inline PValType GetType() const override { return std::make_shared<ValType>("double"); }

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
		inline PValType GetType() const override { return std::make_shared<ValType>("string"); }

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
