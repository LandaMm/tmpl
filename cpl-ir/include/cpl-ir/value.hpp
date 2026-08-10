#pragma once

#include <concepts>
#include <cpl-basics/array.hpp>
#include "type.hpp"

namespace IRGenerate
{

enum class ValueKind
{
	UNKNOWN = 0,
	GLOBAL,
	LOCAL,
	TEMPORAL,
	IMMEDIATE,
	COUNT_VALUE_TYPES,
};

class Value
{
public:
	Value(ValueKind kind, const Type* typ)
		: m_kind(kind), m_typ(typ) { }
	virtual ~Value() = default;

	Value(const Value&) = delete;
	Value& operator=(const Value&) = delete;
	Value(Value&&) = delete;
	Value& operator=(Value&&) = delete;
public:
	inline ValueKind Kind() const noexcept { return m_kind; }
	inline const Type* Typ() const noexcept { return m_typ; }
public:
	template<typename T>
	requires std::derived_from<T, Value>
	inline T* As() noexcept { return static_cast<T*>(this); }

	template<typename T>
	requires std::derived_from<T, Value>
	inline const T* As() const noexcept { return static_cast<const T*>(this); }
private:
	ValueKind m_kind;
	const Type* m_typ;
};

class GlobalValue : public Value
{
public:
	GlobalValue(Array<Byte>&& data, const Type* typ)
		: m_data(data), Value(ValueKind::GLOBAL, typ) { }
public:
	inline const Array<Byte>& Data() const noexcept { return m_data; }
private:
	Array<Byte> m_data;
};

class LocalValue : public Value
{
public:
	LocalValue(const String& name, const Type* typ)
		: m_name(name), Value(ValueKind::LOCAL, typ) {}
public:
	inline const String& Name() const noexcept { return m_name; }
private:
	String m_name;
};

class ImmediateValue : public Value
{
public:
	ImmediateValue(void* immValue, const Type* typ)
		: m_value(immValue), Value(ValueKind::IMMEDIATE, typ) { }
public:
	[[nodiscard]] inline const void* ImmValue() const noexcept { return m_value; }
private:
	void* m_value;
};

using TempValueID = Uint32;

class TemporalValue : public Value
{
public:
	TemporalValue(TempValueID id, const Type* typ)
		: m_id(id), Value(ValueKind::TEMPORAL, typ) { }
public:
	inline TempValueID Id() const noexcept { return m_id; }
public:
	TempValueID m_id;
};

}

