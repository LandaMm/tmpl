#include "cpl-ir/instr.h"

namespace IRGenerate
{

Instr::Instr(InstrOp op) : m_op(op) { }

InstrOp Instr::Type() const noexcept { return m_op; }

} // namespace IRGenerate
