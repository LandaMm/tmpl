#pragma once

#include <cpl-basics/string.hpp>
#include <cpl-basics/array.hpp>

#include "type.hpp"
#include "block.h"

namespace IRGenerate
{

class Scope;

struct FunctionParam
{
	const LocalValue* destination;
	std::optional<const Value*> initialValue;
};


class Function
{
public:
	Function(String name, Array<FunctionParam*>&& params, const FunctionType* funcType, NamedBlock* body, Scope* fnScope)
		: m_name(name), m_params(params), m_funcType(funcType), m_body(body), m_fnScope(fnScope) {}
public:
	[[nodiscard]] inline const String& Name() const noexcept { return m_name; }
	[[nodiscard]] inline const NamedBlock* Body() const noexcept { return m_body; }
	[[nodiscard]] inline const FunctionType* FunctionDescription() const noexcept { return m_funcType; }
	[[nodiscard]] inline const Scope* FunctionScope() const noexcept { return m_fnScope; }
	[[nodiscard]] inline const Array<FunctionParam*>& Params() const noexcept { return m_params; }
private:
	String m_name;
	NamedBlock* m_body;
	Array<FunctionParam*> m_params;
	const FunctionType* m_funcType;
	Scope* m_fnScope;
};

}

