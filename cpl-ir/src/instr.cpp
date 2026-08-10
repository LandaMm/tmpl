#include "cpl-ir/instr.h"

namespace IRGenerate
{

Instr::Instr(InstrOp op, const Value* dst) : m_op(op), m_dst(dst) { }

InstrOp Instr::Type() const noexcept { return m_op; }

} // namespace IRGenerate
