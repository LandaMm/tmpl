#ifndef INTERPRETER_H
#define INTERPRETER_H
#include<memory>
#include"parser.h"
#include"node.h"
#include"node/expression.h"
#include"node/literal.h"

namespace Runtime
{
	using namespace AST::Nodes;
	class Interpreter
	{
	public:
		enum class ValueType
		{
			Integer,
			Float,
			Double,
			String
		};
		class Value
		{
		public:
			inline virtual ValueType GetType() const = 0;
		public:
			template<typename T>
			inline T* Get() { return static_cast<T*>(this); }
		};
		class IntegerValue : public Value
		{
		private:
			std::shared_ptr<int> m_value;
		public:
			IntegerValue(std::shared_ptr<int> value) : m_value(value) {}
			IntegerValue(int value) : m_value(std::make_shared<int>(value)) {}
		public:
			inline ValueType GetType() const override { return ValueType::Integer; }
		public:
			inline std::shared_ptr<int> GetValue() const { return m_value; }
		};
		class FloatValue : public Value
		{
		private:
			std::shared_ptr<float> m_value;
		public:
			FloatValue(std::shared_ptr<float> value) : m_value(value) {}
			FloatValue(float value) : m_value(std::make_shared<float>(value)) {}
		public:
			inline ValueType GetType() const override { return ValueType::Float; }
		public:
			inline std::shared_ptr<float> GetValue() const { return m_value; }
		};
		class DoubleValue : public Value
		{
		private:
			std::shared_ptr<double> m_value;
		public:
			DoubleValue(std::shared_ptr<double> value) : m_value(value) {}
			DoubleValue(double value) : m_value(std::make_shared<double>(value)) {}
		public:
			inline ValueType GetType() const override { return ValueType::Double; }
		public:
			inline std::shared_ptr<double> GetValue() const { return m_value; }
		};
		class StringValue : public Value
		{
		private:
			std::shared_ptr<std::string> m_value;
		public:
			StringValue(std::shared_ptr<std::string> value) : m_value(value) {}
			StringValue(std::string value) : m_value(std::make_shared<std::string>(value)) {}
		public:
			inline ValueType GetType() const override { return ValueType::String; }
		public:
			inline std::shared_ptr<std::string> GetValue() const { return m_value; }
		};
	private:
		std::shared_ptr<Parser> m_parser;
	public:
		Interpreter(std::shared_ptr<Parser> parser) : m_parser(parser) {}
	public:
		std::shared_ptr<Value> Evaluate(std::shared_ptr<Node> node);
		std::shared_ptr<Value> EvaluateExpression(std::shared_ptr<ExpressionNode> expr);
		std::shared_ptr<Value> EvaluateLiteral(std::shared_ptr<LiteralNode> literal);
	};
}

#endif