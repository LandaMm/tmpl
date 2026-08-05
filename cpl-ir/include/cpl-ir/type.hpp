#pragma once

#include <cpl-basics/def.hpp>

namespace IRGenerate
{

class Type
{
public:
	Type(Uint32 size, Uint32 align)
		: m_size(size), m_align(align) { }
public:
	Uint32 Size() const noexcept { return m_size; }
	Uint32 Align() const noexcept { return m_align; }
private:
	Uint32 m_size  = 0;
	Uint32 m_align = 0;
};

} // namespace IRGenerate
