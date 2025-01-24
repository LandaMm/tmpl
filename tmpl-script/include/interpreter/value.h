#ifndef RUNTIME_VALUE_H
#define RUNTIME_VALUE_H
#include<memory>
#include<string>

namespace Runtime
{
	enum class ValueType
	{
		Integer,
		Float,
		Double,
		String,
		Null,
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
}

#endif