#pragma once

#include <cpl-basics/string.hpp>
#include <cpl-basics/array.hpp>

#include "type.hpp"
#include "block.h"

namespace IRGenerate
{

class Function
{
public:
	Function(String name, const FunctionType* funcType, NamedBlock* body)
		: m_name(name), m_funcType(funcType), m_body(body) {}
public:
	[[nodiscard]] inline const String& Name() const noexcept { return m_name; }
	[[nodiscard]] inline const NamedBlock* Body() const noexcept { return m_body; }
	[[nodiscard]] inline const FunctionType* FunctionDescription() const noexcept { return m_funcType; }
private:
	String m_name;
	NamedBlock* m_body;
	const FunctionType* m_funcType;
};

}

