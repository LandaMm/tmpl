#pragma once

#include <cpl-basics/string.hpp>

#include "instr.h"

#include "cplbuild.h"

namespace IRGenerate
{

using BlockIdType = Uint32;

class CPL_EXPORT BasicBlock
{
public:
	explicit BasicBlock();

	BasicBlock(const BasicBlock&) = delete;
	BasicBlock& operator=(const BasicBlock&) = delete;
	BasicBlock(BasicBlock&&) = delete;
	BasicBlock& operator=(BasicBlock&&) = delete;
public:
	void AddInstr(Instr* instr);
private:
	Array<Instr*> m_body;
	BlockIdType m_id;
private:
	static BlockIdType m_counter;
};

class CPL_EXPORT NamedBlock : public BasicBlock
{
public:
	explicit NamedBlock(const String &name);
public:
	const String& GetName() const noexcept;
private:
	String m_name;
};

} // namespace IRGenerate
