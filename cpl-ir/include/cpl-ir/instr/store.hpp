#pragma once

#include "cpl-ir/instr.h"
#include "cpl-ir/symbol.hpp"

#include "cpl-ir/value.hpp"

#include "cplbuild.h"

namespace IRGenerate
{

class CPL_EXPORT StoreInstr : public Instr
{
public:
	explicit StoreInstr(const Value* dest, const Value* src, const IRGenerate::Type* typ)
		: m_src(src), m_typ(typ), Instr(InstrOp::STORE, dest) {}
public:
	inline const Value* Src() const noexcept { return m_src; }
	inline const IRGenerate::Type* Typ() const noexcept { return m_typ; }
private:
	const Value* m_src;
	const IRGenerate::Type* m_typ;
};

} // namespace IRGenerate
