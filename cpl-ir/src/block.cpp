#include "cpl-ir/block.h"

namespace IRGenerate
{

BasicBlock::BasicBlock(BlockIdType id) : m_id(id) { }

const Array<Instr*>& BasicBlock::Body() const noexcept { return m_body; }

void BasicBlock::AddInstr(Instr* instr) { m_body.Emplace(instr); }

} // namespace IRGenerate
