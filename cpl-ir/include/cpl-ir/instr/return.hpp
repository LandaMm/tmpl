#pragma once

#include "cpl-ir/instr.h"
#include "cpl-ir/symbol.hpp"

#include "cpl-ir/value.hpp"

#include "cplbuild.h"

namespace IRGenerate
{

class CPL_EXPORT RetInstr : public Instr
{
public:
	explicit RetInstr(const Value* value)
		: m_retValue(value), Instr(InstrOp::RET, nullptr) { }
public:
	inline const Value* RetValue() const noexcept { return m_retValue; }
private:
	const Value* m_retValue;
};

} // namespace IRGenerate
