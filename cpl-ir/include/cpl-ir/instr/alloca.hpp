#pragma once

#include "cpl-ir/instr.h"
#include "cpl-ir/symbol.hpp"

#include "cpl-ir/value.hpp"

#include "cplbuild.h"

namespace IRGenerate
{

class CPL_EXPORT AllocaInstr : public Instr
{
public:
	explicit AllocaInstr(const LocalValue* dest, const IRGenerate::Type* typ)
		: m_typ(typ), Instr(InstrOp::ALLOCA, dest) { }
public:
	inline const IRGenerate::Type* Typ() const noexcept { return m_typ; }
private:
	const IRGenerate::Type* m_typ;
};

} // namespace IRGenerate
