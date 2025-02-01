

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <string>
#include <unordered_map>
#include <memory>
#include "value.h"

namespace Runtime
{
    using namespace AST;

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

	class Procedure
	{
	private:
		std::shared_ptr<Node> m_body;

	public:
		Procedure(std::shared_ptr<Node> body)
			: m_body(body) {}

	public:
		inline std::shared_ptr<Node> GetBody() { return m_body; }

		friend std::ostream &operator<<(std::ostream &stream, const Procedure &procedure)
		{
			stream << "Procedure(" << procedure.m_body << ")" << std::endl;
			return stream;
		}
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
		bool HasItem(std::string name)
		{
			if (m_declarations.find(name) == m_declarations.end())
			{
				if (m_parent != nullptr)
					return m_parent->HasItem(name);
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

		void AddItem(std::string name, std::shared_ptr<T> var)
		{
			m_declarations.insert(std::make_pair(name, var));
		}

		auto Begin() const { return m_declarations.begin(); }
		auto End() const { return m_declarations.end(); }
		size_t Size() const { return m_declarations.size(); }
	};
}

#endif
