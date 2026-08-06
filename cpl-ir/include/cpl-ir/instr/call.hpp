#pragma once

#include "cpl-ir/instr.h"
#include "cpl-ir/symbol.hpp"

#include "cplbuild.h"

namespace IRGenerate
{

class CPL_EXPORT CallInstr : public Instr
{
public:
	explicit CallInstr(const Symbol &callSymbol)
		: m_callSymbol(callSymbol), Instr(InstrOp::CALL) { }
private:
	Symbol m_callSymbol;
};

} // namespace IRGenerate
