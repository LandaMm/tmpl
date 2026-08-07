#pragma once

#include <cpl-basics/def.hpp>

namespace IRGenerate
{

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
	ALIAS,
	FUNCTION,
	VECTOR,
	COUNT_TYPE_CLASSES,
};

class Type
{
public:
	Type(const String& name, TypeClass typClass)
		: m_name(name), m_typClass(typClass) { }
	virtual ~Type() = default;
public:
	const String& Name() const noexcept { return m_name; }
	const TypeClass& TypClass() const noexcept { return m_typClass; }
private:
	String m_name;
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
	IntegerType(const String& name, TypeLayout layout)
		: Type(name, TypeClass::INTEGER), SizedType(layout) { }
private:
};

class FloatType : public Type, public SizedType
{
public:
	FloatType(const String& name, TypeLayout layout)
		: Type(name, TypeClass::FLOAT), SizedType(layout) { }
private:
	// FloatStandard m_standard = FloatStandard::IEE7...;
};

class FunctionType : public Type
{
public:
	FunctionType(const String& name, const Type* retType)
		: m_retType(retType), Type(name, TypeClass::FUNCTION) { }
public:
	inline const Type* RetType() const noexcept { return m_retType; }
private:
	const Type* m_retType;
};

class PointerType : public Type
{
public:
	PointerType(const Type* underlyingType)
		: m_underlyingType(underlyingType), Type(underlyingType->Name(), TypeClass::POINTER) { }
public:
	const Type* UnderlyingType() const noexcept { return m_underlyingType; }
private:
	const Type* m_underlyingType;
};

class AliasType : public Type
{
public:
	AliasType(const String& name, const Type* originType)
		: m_originType(originType), Type(name, TypeClass::ALIAS) { }
public:
	const Type* OriginType() const noexcept { return m_originType; }
private:
	const Type* m_originType;
};

class VectorType : public Type
{
public:
	VectorType(const Type* itemType, Uint32 itemCount)
		: m_itemType(itemType), m_itemCount(itemCount), Type((String("[") + std::to_string(itemCount).c_str() + " x " + itemType->Name() + "]"), TypeClass::ALIAS) {}
public:
	inline const Type* ItemType() const noexcept { return m_itemType; }
	inline Uint32 ItemCount() const noexcept { return m_itemCount; }
private:
	Uint32 m_itemCount;
	const Type* m_itemType;
};


} // namespace IRGenerate
