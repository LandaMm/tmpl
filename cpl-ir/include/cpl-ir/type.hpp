#pragma once

#include <cpl-basics/def.hpp>

namespace IRGenerate
{

enum class PointerSize
{
	BIT32,
	BIT64
};

struct TypeLayout
{
	Uint32 size, align;
};

enum class TypeClass
{
	UNKNOWN = 0,
	INTEGER,
	FLOAT,
	POINTER,
	FUNCTION,
	COUNT_TYPE_CLASSES,
};

class Type
{
public:
	Type(TypeClass typClass)
		: m_typClass(typClass) { }
	virtual ~Type() = default;
public:
	const TypeClass& TypClass() const noexcept { return m_typClass; }
private:
	TypeClass m_typClass;
};

class SizedType
{
public:
	SizedType(TypeLayout layout)
		: m_layout(layout) { }
public:
	const TypeLayout& Layout() const noexcept { return m_layout; }
private:
	TypeLayout m_layout;
};

class IntegerType : public Type, public SizedType
{
public:
	IntegerType(TypeLayout layout)
		: Type(TypeClass::INTEGER), SizedType(layout) { }
private:
};

class FloatType : public Type, public SizedType
{
public:
	FloatType(TypeLayout layout)
		: Type(TypeClass::FLOAT), SizedType(layout) { }
private:
	// FloatStandard m_standard = FloatStandard::IEE7...;
};

class FunctionType : public Type
{
public:
	FunctionType()
		: Type(TypeClass::FUNCTION) { }
private:
};

class PointerType : public Type
{
public:
	PointerType(const Type* underlyingType)
		: m_underlyingType(underlyingType), Type(TypeClass::POINTER) { }
private:
	const Type* m_underlyingType;
};

} // namespace IRGenerate
