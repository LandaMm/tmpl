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
	explicit BasicBlock(BlockIdType id);

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
private: // for TemporalValue
	TempValueID m_tempValueCounter = 0;
};

} // namespace IRGenerate
