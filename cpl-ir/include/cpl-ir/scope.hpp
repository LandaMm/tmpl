#pragma once

#include <map>
#include "type.hpp"
#include "value.hpp"
#include "function.hpp"

namespace IRGenerate
{

class Scope
{
public:
	Scope() = default;

	Scope(const Scope&) = delete;
	Scope& operator=(const Scope&) = delete;
	Scope(Scope&&) = delete;
	Scope& operator=(Scope&&) = delete;
public:
	void AddType(const String& name, Type* typ)
	{
		m_localTypes.insert({ name, typ });
	}
	const Type* FindType(const String& name) const noexcept
	{
		if (m_localTypes.find(name) == m_localTypes.end()) return nullptr;
		return m_localTypes.at(name);
	}

	void AddLocal(const LocalValue* value)
	{
		m_locals.insert({ value->Name(), value });
	}
	const LocalValue* FindLocal(const String& name) const noexcept
	{
		if (m_locals.find(name) == m_locals.end()) return nullptr;
		return m_locals.at(name);
	}

	void AddFunction(const String& name, Function* function)
	{
		m_functions.insert({ name, function });
	}
public:
	[[nodiscard]] inline const std::map<String, const LocalValue*>& Locals() const noexcept { return m_locals; };
	[[nodiscard]] inline const std::map<String, Type*>& Types() const noexcept { return m_localTypes; }
	[[nodiscard]] inline const std::map<String, Function*>& Functions() const noexcept { return m_functions; }
private:
	std::map<String, const LocalValue*> m_locals;
	std::map<String, Type*> m_localTypes;
	std::map<String, Function*> m_functions;
};

} // namespace IRGenerate
