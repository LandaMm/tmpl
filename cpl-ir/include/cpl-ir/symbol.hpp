#pragma once

#include <cpl-basics/string.hpp>

#include "type.hpp"

namespace IRGenerate
{

enum class SymbolKind
{
	UNKNOWN = 0,
	VARIABLE,
	FUNCTION,
	COUNT_SYMBOL_TYPES,
};

enum class SymbolOrigin
{
	UNKNOWN = 0,
	LOCAL,
	FOREIGN,
	COUNT_SYMBOL_ORIGINS,
};

class Symbol
{
public:
	explicit Symbol(SymbolKind kind, const Type* typ, SymbolOrigin origin, const String &name)
		: m_kind(kind), m_type(typ), m_origin(origin), m_name(name) {}
	virtual ~Symbol() = default;
public:
	SymbolKind Kind() const noexcept { return m_kind; }
	SymbolOrigin Origin() const noexcept { return m_origin; }
	const Type* Typ() const noexcept { return m_type; }
	const String& Name() const { return m_name; }
private:
	SymbolKind m_kind;
	SymbolOrigin m_origin;
	const Type* m_type;
	String m_name;
};

namespace Symbols
{

class LocalVariable : public Symbol
{
public:
	explicit LocalVariable(const String& name, const Type* typ, const Value* initialValue)
		: m_initialValue(initialValue), Symbol(SymbolKind::VARIABLE, typ, SymbolOrigin::LOCAL, name)
	{ }
public:
	[[nodiscard]] inline const Value* InitialValue() const noexcept { return m_initialValue; }
private:
	const Value* m_initialValue;
};

class ExternalVariable : public Symbol
{
public:
	explicit ExternalVariable(const String& name, const Type* typ)
		: Symbol(SymbolKind::VARIABLE, typ, SymbolOrigin::FOREIGN, name)
	{ }
};

class Function : public Symbol
{
public:
	explicit Function(const String& name, const FunctionType* typ, SymbolOrigin origin)
		: Symbol(SymbolKind::FUNCTION, typ, origin, name)
	{ }
public:
	[[nodiscard]] inline const FunctionType* FunctionDescription() const noexcept { auto func = dynamic_cast<const FunctionType*>(Typ()); assert(func); return func; }
};

} // namespace Symbols

} // namespace IRGenerate
