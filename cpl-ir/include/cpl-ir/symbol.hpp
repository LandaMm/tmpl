#pragma once

#include "cpl-basics/string.hpp"

#include "type.hpp"

namespace IRGenerate
{

enum class SymbolType
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
	explicit Symbol(SymbolType typ, const Type* valueType, SymbolOrigin origin, const String &name)
		: m_type(typ), m_valueType(valueType), m_origin(origin), m_name(name) {}
public:
	SymbolType SymType() const noexcept { return m_type; }
	const Type* ValueType() const noexcept { return m_valueType; }
	SymbolOrigin Origin() const noexcept { return m_origin; }
	const String& Name() const { return m_name; }
private:
	SymbolType m_type;
	SymbolOrigin m_origin;
	const Type* m_valueType;
	String m_name;
};

} // namespace IRGenerate
