#pragma once

#include "value.hpp"

#include "cplbuild.h"

namespace IRGenerate
{

enum class InstrOp
{
	NONE = 0,
	CALL,
	ALLOCA,
	STORE,
	LOAD,
	COUNT_INSTR_OPS,
};

class CPL_EXPORT Instr
{
public:
	explicit Instr(InstrOp op, const Value* dst = nullptr);
	virtual ~Instr() = default;

	Instr(const Instr&) = default;
	Instr& operator=(const Instr&) = default;
	Instr(Instr&&) = delete;
	Instr& operator=(Instr&&) = delete;

	template<typename T>
	requires std::derived_from<T, Instr>
	inline const T* As() const noexcept { return dynamic_cast<const T*>(this); }
public:
	inline bool Valued() const noexcept { return m_dst != nullptr; }
	inline const Value* Dest() const noexcept { return m_dst; }
	InstrOp Type() const noexcept;
private:
	InstrOp m_op;
	const Value* m_dst;
};

} // namespace IRGenerate
