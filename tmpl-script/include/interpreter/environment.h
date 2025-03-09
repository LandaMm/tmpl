

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "value.h"

namespace Runtime
{
    using namespace AST;

	class Variable
	{
	private:
		// TODO:
		// `std::string m_typename;` - for complex type names, e.g. Point<int> instead of object {x: int, y: int}
		PValType m_type;
		std::shared_ptr<Value> m_value;
		bool m_editable;

	public:
		Variable(PValType type, std::shared_ptr<Value> value, bool editable)
			: m_type(type), m_value(value), m_editable(editable) {}

	public:
		inline std::shared_ptr<Value> GetValue() const { return m_value; }
        inline PValType GetType() const { return m_type; }
        inline bool IsEditable() const { return m_editable; }

    public:
        void SetValue(std::shared_ptr<Value> newValue) { m_value = newValue; }
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

    class FnParam
    {
    private:
        PValType m_type;
        std::string m_name;
    public:
        FnParam(PValType type, std::string name) : m_type(type), m_name(name) { }
        ~FnParam() = default;
    public:
        inline PValType GetType() const { return m_type; }
        inline std::string GetName() const { return m_name; }
    };

    class FnGeneric
    {
    private:
        std::string m_name;
        AST::Location m_loc;
        // TODO: base and default type
    public:
        FnGeneric(std::string name, AST::Location loc) : m_name(name), m_loc(loc) { }
        ~FnGeneric() = default;
    public:
        inline std::string GetName() const { return m_name; }
        inline AST::Location GetLocation() const { return m_loc; }
    };

    enum class FnModifier
    {
        Regular,
        Type,
        Construct,
        Cast
    };

	class Fn
	{
	private:
		std::shared_ptr<Node> m_body;

        std::vector<std::shared_ptr<FnParam>> m_params;
        PValType m_ret_type;

        FnModifier m_modifier;

        std::vector<std::shared_ptr<FnGeneric>> m_generics;

        std::string m_module_name;
        bool m_exported;
        Location m_loc;
        bool m_externed;
    private:
        size_t m_index;

	public:
		Fn(std::shared_ptr<Node> body, PValType retType, FnModifier modifier, std::string module, bool exported, bool externed, Location loc)
			: m_body(body),
            m_ret_type(retType),
            m_params(std::vector<std::shared_ptr<FnParam>>()),
            m_index(0),
            m_modifier(modifier),
            m_module_name(module), m_exported(exported), m_externed(externed),
            m_loc(loc) {}

    public:
        void AddParam(std::shared_ptr<FnParam> param) { m_params.push_back(param); }
        void AddGeneric(std::shared_ptr<FnGeneric> generic) { m_generics.push_back(generic); }

		void SetReturnType(PValType newTyp) { m_ret_type = newTyp; }
    public:
        inline std::shared_ptr<FnParam> GetParam(unsigned int index) const { return m_params[index]; }
        inline size_t GetParamsSize() const { return m_params.size(); }

        inline FnModifier GetModifier() const { return m_modifier; }
        
        inline std::shared_ptr<FnGeneric> GetGeneric(unsigned int index) const { return m_generics[index]; }
        inline unsigned int GetGenericsSize() const { return m_generics.size(); }

        inline std::vector<std::shared_ptr<FnGeneric>>* GetGenIterator() { return &m_generics; }

	public:
		inline std::shared_ptr<Node> GetBody() const { return m_body; }
        inline PValType GetReturnType() const { return m_ret_type; }
        inline std::string GetModuleName() const { return m_module_name; }
        inline bool IsExported() const { return m_exported; }
        inline bool IsExterned() const { return m_externed; }
        inline Location GetLocation() const { return m_loc; }

		friend std::ostream &operator<<(std::ostream &stream, const Fn &fn)
		{
			stream << "Fn(" << fn.m_body << ")" << std::endl;
			return stream;
		}
	};

	template <typename T, typename K = std::string>
	class Environment
	{
	private:
		std::shared_ptr<Environment<T, K>> m_parent;
		std::unordered_map<K, std::shared_ptr<T>> m_declarations;

	public:
		Environment()
			: m_parent(nullptr) {}
		Environment(std::shared_ptr<Environment<T>> parentEnv)
			: m_parent(parentEnv) {}

	public:
        inline std::shared_ptr<Environment<T>> GetParent() const { return m_parent; }
		bool HasItem(K name)
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

        bool Contains(K name)
        {
			return m_declarations.find(name) != m_declarations.end();
        }

		std::shared_ptr<T> LookUp(K name)
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

		void AddItem(K name, std::shared_ptr<T> var)
		{
			m_declarations.insert(std::make_pair(name, var));
		}

		auto Begin() const { return m_declarations.begin(); }
		auto End() const { return m_declarations.end(); }
		size_t Size() const { return m_declarations.size(); }
	};
}

#endif
