

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <string>
#include <unordered_map>
#include <memory>
#include "value.h"

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
			: m_type(type), m_value(value), m_editable(editable) {}

	public:
		inline std::shared_ptr<Value> GetValue() { return m_value; }
	};

	template <typename T>
	class Environment
	{
	private:
		std::shared_ptr<Environment<T>> m_parent;
		std::unordered_map<std::string, std::shared_ptr<T>> m_declarations;

	public:
		Environment()
			: m_parent(nullptr) {}
		Environment(std::shared_ptr<Environment<T>> parentEnv)
			: m_parent(parentEnv) {}

	public:
		bool HasVariable(std::string name)
		{
			if (m_declarations.find(name) == m_declarations.end())
			{
				if (m_parent != nullptr)
					return m_parent->HasVariable(name);
				else
					return false;
			}

			return true;
		}

		std::shared_ptr<T> LookUp(std::string name)
		{
			if (m_declarations.find(name) == m_declarations.end())
			{
				if (m_parent != nullptr)
					return m_parent->LookUp(name);
				else
					return nullptr;
			}
			else
				return m_declarations.at(name);
		}

		void AddVariable(std::string name, std::shared_ptr<T> var)
		{
			// TODO: raise error when var exists already
			m_declarations.insert(std::make_pair(name, var));
		}
	};
}

#endif
