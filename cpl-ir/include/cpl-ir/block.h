#pragma once

#include <cpl-basics/string.hpp>

#include "instr.h"
#include "value.hpp"

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
	const Array<Instr*>& Body() const noexcept;
	void AddInstr(Instr* instr);
public:
	[[nodiscard]] inline TempValueID NextTempValueId() noexcept { return m_tempValueCounter++; }
	[[nodiscard]] inline BlockIdType Id() const noexcept { return m_id; }
private:
	Array<Instr*> m_body;
	BlockIdType m_id;
private:
	static BlockIdType m_counter;
private: // for TemporalValue
	TempValueID m_tempValueCounter = 0;
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
