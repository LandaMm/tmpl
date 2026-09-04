#pragma once

#include "cpl-ir/instr.h"

#include "cpl-ir/value.hpp"

#include "cplbuild.h"

namespace IRGenerate
{

class CPL_EXPORT AddInstr : public Instr
{
public:
	explicit AddInstr(const Value* dest, const Value* left, const Value* right)
		: m_left(left), m_right(right), Instr(InstrOp::ADD, dest) {}
public:
	inline const Value* Left() const noexcept { return m_left; }
	inline const Value* Right() const noexcept { return m_right; }
private:
	const Value* m_left;
	const Value* m_right;
};

class CPL_EXPORT SubInstr : public Instr
{
public:
	explicit SubInstr(const Value* dest, const Value* left, const Value* right)
		: m_left(left), m_right(right), Instr(InstrOp::SUB, dest) {}
public:
	inline const Value* Left() const noexcept { return m_left; }
	inline const Value* Right() const noexcept { return m_right; }
private:
	const Value* m_left;
	const Value* m_right;
};

class CPL_EXPORT MulInstr : public Instr
{
public:
	explicit MulInstr(const Value* dest, const Value* left, const Value* right)
		: m_left(left), m_right(right), Instr(InstrOp::MUL, dest) {}
public:
	inline const Value* Left() const noexcept { return m_left; }
	inline const Value* Right() const noexcept { return m_right; }
private:
	const Value* m_left;
	const Value* m_right;
};

class CPL_EXPORT DivInstr : public Instr
{
public:
	explicit DivInstr(const Value* dest, const Value* left, const Value* right)
		: m_left(left), m_right(right), Instr(InstrOp::DIV, dest) {}
public:
	inline const Value* Left() const noexcept { return m_left; }
	inline const Value* Right() const noexcept { return m_right; }
private:
	const Value* m_left;
	const Value* m_right;
};

class CPL_EXPORT NegInstr : public Instr
{
public:
	explicit NegInstr(const Value* dest, const Value* target)
		: m_target(target), Instr(InstrOp::NEG, dest) { }
public:
	inline const Value* Target() const noexcept { return m_target; }
private:
	const Value* m_target = nullptr;
};

} // namespace IRGenerate

