#pragma once

#include "cpl-ir/instr.h"
#include "cpl-ir/symbol.hpp"

#include "cpl-ir/value.hpp"

#include "cplbuild.h"

namespace IRGenerate
{

class CPL_EXPORT CallInstr : public Instr
{
public:
	explicit CallInstr(const Symbol* callSymbol, Array<const Value*>&& args, const IRGenerate::Type* retType)
		: m_callSymbol(callSymbol), m_args(args), m_retType(retType), Instr(InstrOp::CALL) {}
public:
	inline const Symbol* CallSymbol() const noexcept { return m_callSymbol; }
	inline const Array<const Value*>& Args() const noexcept { return m_args; }
	inline const IRGenerate::Type* RetType() const noexcept { return m_retType; }
private:
	const Symbol* m_callSymbol;
	const IRGenerate::Type* m_retType;
	Array<const Value*> m_args;
};

} // namespace IRGenerate
