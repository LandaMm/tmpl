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

	void AddSymbol(Symbol* value)
	{
		m_symbols.insert({ value->Name(), value });
	}
	const Symbol* FindSymbol(const String& name) const noexcept
	{
		if (m_symbols.find(name) == m_symbols.end()) return nullptr;
		return m_symbols.at(name);
	}

	void AddFunction(const String& name, Function* function)
	{
		m_functions.insert({ name, function });
	}
	const Function* FindFunction(const String& name) const noexcept
	{
		if (m_functions.find(name) == m_functions.end()) return nullptr;
		return m_functions.at(name);
	}
public:
	[[nodiscard]] inline const std::map<String, const LocalValue*>& Locals() const noexcept { return m_locals; };
	[[nodiscard]] inline const std::map<String, Type*>& Types() const noexcept { return m_localTypes; }
	[[nodiscard]] inline const std::map<String, Function*>& Functions() const noexcept { return m_functions; }
	[[nodiscard]] inline const std::map<String, Symbol*>& Symbols() const noexcept { return m_symbols; }
private:
	std::map<String, Symbol*> m_symbols;
	std::map<String, const LocalValue*> m_locals;
	std::map<String, Type*> m_localTypes;
	std::map<String, Function*> m_functions;
};

} // namespace IRGenerate
