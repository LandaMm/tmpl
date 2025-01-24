

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include<string>
#include<unordered_map>
#include<memory>
#include"value.h"

namespace Runtime
{
	class Variable
	{
	private:
		// TODO:
		// `std::string m_typename;` - for complex type names, e.g. Point<int> instead of object {x: int, y: int}
		ValueType m_type;
		std::shared_ptr<Value> m_value;
		bool m_editable;
	public:
		Variable(ValueType type, std::shared_ptr<Value> value, bool editable)
			: m_type(type), m_value(value), m_editable(editable) { }
	public:
		inline std::shared_ptr<Value> GetValue() { return m_value; }
	};

	class Environment
	{
	private:
		std::shared_ptr<Environment> m_parent;
		std::unordered_map<std::string, std::shared_ptr<Variable>> m_variables;
	public:
		Environment()
			: m_parent(nullptr) { }
		Environment(std::shared_ptr<Environment> parentEnv)
			: m_parent(parentEnv) { }
	public:
		std::shared_ptr<Variable> LookUp(std::string name);
		bool HasVariable(std::string name);
		void AddVariable(std::string name, std::shared_ptr<Variable> var);
	};
}

#endif
