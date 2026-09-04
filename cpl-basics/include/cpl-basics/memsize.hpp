#pragma once

#include "string.hpp"

class MemSize
{
private:
	constexpr explicit MemSize(Uint32 bits)
		: m_bits(bits) { }
public:
	MemSize() : m_bits(0) { }
public:
	constexpr inline static MemSize FromBytes(Uint32 bytes) noexcept { return MemSize(bytes * 8); }
	constexpr inline static MemSize FromBits(Uint32 bits) noexcept { return MemSize(bits); }
public:
	inline Uint32 Bytes() const noexcept { assert(m_bits % 8 == 0); return m_bits / 8; }
	constexpr inline Uint32 Bits() const noexcept { return m_bits; }
	inline String ToString() const { return std::to_string(m_bits); }
public:
	MemSize operator+(const MemSize& other) const noexcept { return MemSize::FromBits(m_bits + other.Bits()); }
	MemSize operator-(const MemSize& other) const noexcept { return MemSize::FromBits(m_bits - other.Bits()); }
	MemSize operator*(Uint32 value) const noexcept { return MemSize::FromBits(m_bits * value); }
	MemSize operator%(MemSize alignment) const noexcept { return MemSize::FromBytes((alignment.Bytes() - (Bytes() % alignment.Bytes())) % alignment.Bytes()); }
	void operator*=(Uint32 value) noexcept { m_bits *= value; }
	void operator+=(const MemSize& other) noexcept { m_bits += other.Bits(); }
	void operator-=(const MemSize& other) noexcept { m_bits -= other.Bits(); }
	bool operator==(const MemSize& other) const noexcept { return m_bits == other.Bits(); }
	bool operator!=(const MemSize& other) const noexcept { return m_bits != other.Bits(); }
	bool operator<(const MemSize& other) const noexcept { return m_bits < other.Bits(); }
	bool operator>(const MemSize& other) const noexcept { return m_bits > other.Bits(); }
	bool operator>=(const MemSize& other) const noexcept { return m_bits >= other.Bits(); }
	bool operator<=(const MemSize& other) const noexcept { return m_bits <= other.Bits(); }
private:
	Uint32 m_bits;
};
