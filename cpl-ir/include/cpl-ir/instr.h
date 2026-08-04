#pragma once

#include "cplbuild.h"

namespace IRGenerate
{

enum class InstrOp
{
	NONE = 0,
	CALL,
	LOAD,
};

class CPL_EXPORT Instr
{
public:
	explicit Instr(InstrOp op);
	virtual ~Instr() = default;

	Instr(const Instr&) = default;
	Instr& operator=(const Instr&) = default;
	Instr(Instr&&) = delete;
	Instr& operator=(Instr&&) = delete;
public:
	InstrOp Type() const noexcept;
private:
	InstrOp m_op;
};

} // namespace IRGenerate
