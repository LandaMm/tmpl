#pragma once

#include "cpl-ir/instr.h"
#include "cpl-ir/symbol.hpp"
#include "cpl-ir/block.h"

#include "cpl-ir/value.hpp"

#include "cplbuild.h"

namespace IRGenerate
{

class CPL_EXPORT JmpInstr : public Instr
{
public:
	explicit JmpInstr(BlockIdType blockId)
		: m_blockId(blockId), Instr(InstrOp::JMP, nullptr) { }
public:
	inline const BlockIdType BlockId() const noexcept { return m_blockId; }
private:
	BlockIdType m_blockId;
};

class CPL_EXPORT JmpIfInstr : public Instr
{
public:
	explicit JmpIfInstr(BlockIdType blockId, const Value* condition)
		: m_blockId(blockId), m_condition(condition), Instr(InstrOp::JMP_IF, nullptr) {}
public:
	inline const BlockIdType BlockId() const noexcept { return m_blockId; }
	inline const Value* Condition() const noexcept { return m_condition; }
private:
	BlockIdType m_blockId;
	const Value* m_condition;
};

class CPL_EXPORT BranchInstr : public Instr
{
public:
	explicit BranchInstr(BlockIdType trueBlock, BlockIdType falseBlock, const Value* condition)
		: m_trueBlockId(trueBlock), m_falseBlockId(falseBlock), m_condition(condition), Instr(InstrOp::BRANCH, nullptr) {}
public:
	inline const BlockIdType TrueBlockId() const noexcept { return m_trueBlockId; }
	inline const BlockIdType FalseBlockId() const noexcept { return m_falseBlockId; }
	inline const Value* Condition() const noexcept { return m_condition; }
private:
	BlockIdType m_trueBlockId;
	BlockIdType m_falseBlockId;
	const Value* m_condition;
};

} // namespace IRGenerate
