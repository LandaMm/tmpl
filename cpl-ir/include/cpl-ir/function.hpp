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
	Function(String name, Array<FunctionParam*>&& params, const FunctionType* funcType, Scope* fnScope)
		: m_name(name), m_params(params), m_funcType(funcType), m_fnScope(fnScope) {}
public:
	[[nodiscard]] inline const String& Name() const noexcept { return m_name; }
	[[nodiscard]] inline BasicBlock* CurrentBlock() noexcept {
		if (m_blocks.Empty())
			return nullptr;
		else
			return m_blocks[m_blocks.Size() - 1];
	}
	[[nodiscard]] inline const Array<BasicBlock*>& Blocks() const noexcept { return m_blocks; }
	[[nodiscard]] inline const FunctionType* FunctionDescription() const noexcept { return m_funcType; }
	[[nodiscard]] inline const Scope* FunctionScope() const noexcept { return m_fnScope; }
	[[nodiscard]] inline const Array<FunctionParam*>& Params() const noexcept { return m_params; }
public:
	inline void AddBlock(BasicBlock* block) { m_blocks.Push(block); }
private:
	String m_name;
	Array<BasicBlock*> m_blocks;
	Array<FunctionParam*> m_params;
	const FunctionType* m_funcType;
	Scope* m_fnScope;
};

}

