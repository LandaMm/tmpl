#include "cpl-ir/block.h"

namespace IRGenerate
{

BasicBlock::BasicBlock() : m_id(m_counter++) { }

const Array<Instr*>& BasicBlock::Body() const noexcept { return m_body; }

void BasicBlock::AddInstr(Instr* instr) { m_body.Emplace(instr); }

BlockIdType BasicBlock::m_counter = 0;

NamedBlock::NamedBlock(const String& name) : m_name(name) { }

const String& NamedBlock::GetName() const noexcept { return m_name; }

} // namespace IRGenerate
