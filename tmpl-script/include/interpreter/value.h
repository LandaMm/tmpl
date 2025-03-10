#ifndef RUNTIME_VALUE_H
#define RUNTIME_VALUE_H
#include <memory>
#include <ostream>
#include <string>
#include <cassert>
#include <vector>
#include "../node/logical.h"
#include "../node/expression.h"
#include "include/iterator.h"

namespace Runtime
{
    struct CustomValueType
    {
    private:
        std::string m_name;
        std::vector<std::shared_ptr<CustomValueType>> m_generics;

    public:
        CustomValueType(std::string name)
            : m_name(name) { }
        CustomValueType(const CustomValueType& typ)
            : m_name(typ.m_name), m_generics(typ.m_generics) {}
        CustomValueType(const std::shared_ptr<CustomValueType>& typ)
            : m_name(typ->m_name)
        {
            auto it = Common::Iterator(typ->GetGenericsSize());
            while (it.HasItems())
            {
                AddGeneric(typ->GetGeneric(it.GetPosition()));
                it.Next();
            }
        }

    public:
        bool Compare(const CustomValueType& other);
        bool IsMixed() const { return m_name == "#BUILTIN_MIXED"; }

    public:
        inline std::string GetName() const { return m_name; }
        void SetName(std::string newName) { m_name = newName; }

    public:
        void AddGeneric(std::shared_ptr<CustomValueType> generic) { m_generics.push_back(generic); }
        void SetGeneric(unsigned int index, std::shared_ptr<CustomValueType> generic) { m_generics[index] = generic; }
        inline std::shared_ptr<CustomValueType> GetGeneric(unsigned int index) 
            const { return m_generics[index]; }
        inline unsigned int GetGenericsSize() const { return m_generics.size(); }


    public:
        friend std::ostream &operator<<(std::ostream& stream, const CustomValueType &x);
    };

    typedef CustomValueType ValType;
    typedef std::shared_ptr<ValType> PValType;

	class Value
	{
    private:
        PValType m_value_type;

	public:
		inline PValType GetType() const { return m_value_type; };

	public:
		virtual std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) = 0;
		virtual std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) = 0;

	public:
		virtual ~Value() = default;

    public:
        Value(PValType valType) : m_value_type(valType) { }

	public:
		virtual std::string format() const = 0;

    public:
        virtual std::shared_ptr<Value> Clone() const = 0;
    
    public:
        void SetType(PValType newTyp) { m_value_type = newTyp; }
	};

	std::ostream &operator<<(std::ostream &stream, const Value &value);
	std::ostream &operator<<(std::ostream &stream, const std::shared_ptr<Value> &value);

	class VoidValue : public Value
	{
	public:
		VoidValue() : Value(std::make_shared<ValType>("void")) { }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

	public:
		std::string format() const override;

    public:
        std::shared_ptr<Value> Clone() const override
        {
            return std::make_shared<VoidValue>();
        }
	};

	class BoolValue : public Value
	{
	private:
		bool m_value;

	public:
		BoolValue(bool value) : m_value(value), Value(std::make_shared<ValType>("bool")) {}

    public:
        BoolValue(const BoolValue* val) : Value(val->GetType())
        {
            m_value = val->m_value;
        }

	public:
		inline bool GetValue() const { return m_value; }
        void SetValue(bool newValue) { m_value = newValue; }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

	public:
		std::string format() const override;

    public:
        std::shared_ptr<Value> Clone() const override
        {
            return std::make_shared<BoolValue>(this);
        }
	};

	class IntegerValue : public Value
	{
	private:
		std::shared_ptr<int> m_value;

	public:
		IntegerValue(std::shared_ptr<int> value) : m_value(value), Value(std::make_shared<ValType>("int")) {}
		IntegerValue(int value) : m_value(std::make_shared<int>(value)), Value(std::make_shared<ValType>("int")) {}

    public:
        IntegerValue(const IntegerValue* val) : Value(val->GetType())
        {
            m_value = std::make_shared<int>(*val->m_value);
        }

	public:
		inline std::shared_ptr<int> GetValue() const { return m_value; }
        void SetValue(int newValue) { *m_value = newValue; }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

	public:
		std::string format() const override;

    public:
        std::shared_ptr<Value> Clone() const override
        {
            return std::make_shared<IntegerValue>(this);
        }
	};

	class ListValue : public Value
	{
	private:
		std::vector<std::shared_ptr<Value>> m_value;
        PValType m_items_type;

	public:
		ListValue(PValType itemsType)
            : m_value(std::vector<std::shared_ptr<Value>>()),
              m_items_type(itemsType),
              Value(std::make_shared<ValType>("list"))
        {
            auto finalType = std::make_shared<ValType>("list");
            finalType->AddGeneric(itemsType);
            SetType(finalType);
        }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

    public:
        ListValue(const ListValue* val) : Value(val->GetType())
        {
            // TODO: think if we want deep copy
            m_value = val->m_value;
            m_items_type = val->m_items_type;
        }

    public:
        void AddItem(std::shared_ptr<Value> item);
        std::shared_ptr<Value> GetItem(std::shared_ptr<IntegerValue> indexVal);

	public:
		std::string format() const override;

    public:
        std::shared_ptr<Value> Clone() const override
        {
            return std::make_shared<ListValue>(this);
        }
	};

	class FloatValue : public Value
	{
	private:
		std::shared_ptr<float> m_value;

	public:
		FloatValue(std::shared_ptr<float> value) : m_value(value), Value(std::make_shared<ValType>("float")) {}
		FloatValue(float value) : m_value(std::make_shared<float>(value)), Value(std::make_shared<ValType>("float")) {}

	public:
		inline std::shared_ptr<float> GetValue() const { return m_value; }
        void SetValue(float newValue) { *m_value = newValue; }

    public:
        FloatValue(const FloatValue* val) : Value(val->GetType())
        {
            m_value = std::make_shared<float>(*val->m_value);
        }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

	public:
		std::string format() const override;

    public:
        std::shared_ptr<Value> Clone() const override
        {
            return std::make_shared<FloatValue>(this);
        }
	};
	class DoubleValue : public Value
	{
	private:
		std::shared_ptr<double> m_value;

	public:
		DoubleValue(std::shared_ptr<double> value) : m_value(value), Value(std::make_shared<ValType>("double")) {}
		DoubleValue(double value) : m_value(std::make_shared<double>(value)), Value(std::make_shared<ValType>("double")) {}

	public:
		inline std::shared_ptr<double> GetValue() const { return m_value; }
        void SetValue(double newValue) { *m_value = newValue; }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

    public:
        DoubleValue(const DoubleValue* val) : Value(val->GetType())
        {
            m_value = std::make_shared<double>(*val->m_value);
        }

	public:
		std::string format() const override;

    public:
        std::shared_ptr<Value> Clone() const override
        {
            return std::make_shared<DoubleValue>(this);
        }
	};
	class StringValue : public Value
	{
	private:
		std::shared_ptr<std::string> m_value;

	public:
		StringValue(std::shared_ptr<std::string> value) : m_value(value), Value(std::make_shared<ValType>("string")) {}
		StringValue(std::string value) : m_value(std::make_shared<std::string>(value)), Value(std::make_shared<ValType>("string")) {}

	public:
		inline std::shared_ptr<std::string> GetValue() const { return m_value; }
        void SetValue(std::string newValue) { *m_value = newValue; }

	public:
		std::shared_ptr<Value> Compare(std::shared_ptr<Value> right, AST::Nodes::Condition::ConditionType condition) override;
		std::shared_ptr<Value> Operate(std::shared_ptr<Value> right, AST::Nodes::ExpressionNode::OperatorType opType) override;

    public:
        StringValue(const StringValue* val) : Value(val->GetType())
        {
            m_value = std::make_shared<std::string>(*val->m_value);
        }

	public:
		std::string format() const override;

    public:
        std::shared_ptr<Value> Clone() const override
        {
            return std::make_shared<StringValue>(this);
        }
	};
}

#endif
