#pragma once

#include "cpl-basics/string.hpp"
#include "cpl-basics/array.hpp"

#include "type.hpp"
#include "block.h"

namespace IRGenerate
{

struct FunctionParam
{
	String name;
	Type* typ;
};

class Function
{
public:
	Function(String name, Array<FunctionParam>&& params, NamedBlock* body, Type* retType)
		: m_name(name), m_params(params), m_body(body), m_retType(retType) {}
public:
	const String& Name() const noexcept { return m_name; }
	const Array<FunctionParam>& Params() const noexcept { return m_params; }
	Type* RetType() const noexcept { return m_retType; }
private:
	String m_name;
	NamedBlock* m_body;
	Array<FunctionParam> m_params;
	Type* m_retType;
};

}

