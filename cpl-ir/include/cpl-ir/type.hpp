#pragma once

#include <optional>
#include <iostream>

#include <cpl-basics/def.hpp>
#include <cpl-basics/string.hpp>
#include <cpl-basics/memsize.hpp>

namespace IRGenerate
{

struct TypeLayout
{
	MemSize size, align;
};

// TODO: FIXME: pointer size depending on the target system arch
constexpr TypeLayout POINTER_SIZE = {MemSize::FromBits(64), MemSize::FromBits(64)};

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
	template<typename T>
	requires std::derived_from<T, Type>
	const T* As() const noexcept { return dynamic_cast<const T*>(this); }

	const String& Name() const noexcept { return m_name; }
	const TypeClass& TypClass() const noexcept { return m_typClass; }
public:
	friend bool operator==(const Type& lhs, const Type& rhs) noexcept;

	virtual bool operator!=(const Type& other) const noexcept
	{
		return !operator==(*this, other);
	}
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
	struct Param
	{
		String name;
		const Type* typ;
	};
public:
	FunctionType(const String& name, Array<Param>&& params, const Type* retType)
		: m_params(params), m_retType(retType), Type(name, TypeClass::FUNCTION) { }
public:
	inline const Array<Param>& Params() const noexcept { return m_params; }
	inline const Type* RetType() const noexcept { return m_retType; }
private:
	Array<Param> m_params;
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
		: m_itemType(itemType), m_itemCount(itemCount), Type((String("[") + std::to_string(itemCount).c_str() + " x " + itemType->Name() + "]"), TypeClass::VECTOR) {}
public:
	inline const Type* ItemType() const noexcept { return m_itemType; }
	inline Uint32 ItemCount() const noexcept { return m_itemCount; }
private:
	Uint32 m_itemCount;
	const Type* m_itemType;
};

class TypeResolver
{
public:
	TypeResolver() = delete;
public:
	static bool TwoAre(const Type* a, const Type* b, TypeClass ac, TypeClass bc) noexcept
	{
		return (a->TypClass() == ac && b->TypClass() == bc) || (a->TypClass() == bc && b->TypClass() == ac);
	}

	static const IntegerType* Integer(const Type* typ)
	{
		if (typ->TypClass() == TypeClass::INTEGER) return dynamic_cast<const IntegerType*>(typ);
		if (typ->TypClass() == TypeClass::ALIAS)
		{
			const AliasType* alias = dynamic_cast<const AliasType*>(typ);
			assert(alias);
			return Integer(alias);
		}
		return nullptr;
	}

	// Should not just return bool, but say if type required downgrading/upgrading/alias resolving and etc.
	static bool Identical(const Type* lhs, const Type* rhs)
	{
		if (lhs == rhs) return true;
 
		if (lhs->TypClass() == rhs->TypClass() && lhs->TypClass() == TypeClass::INTEGER)
		{
			auto lhsLayout = ResolveTypeSize(lhs);
			auto rhsLayout = ResolveTypeSize(rhs);
			if (lhsLayout.size != rhsLayout.size)
			{
				// TODO: better error (warning)
				std::cerr << "[WARNING] possible loss of integer data" << std::endl;
			}
			return true;
		}

		// example: *TYPE == *TYPE
		if (lhs->TypClass() == rhs->TypClass() && lhs->TypClass() == TypeClass::POINTER)
		{
			return Identical(lhs->As<PointerType>()->UnderlyingType(), rhs->As<PointerType>()->UnderlyingType());
		}

		// example: *TYPE == [n x TYPE]
		if (lhs->TypClass() == TypeClass::POINTER && rhs->TypClass() == TypeClass::VECTOR)
		{
			auto pointer = dynamic_cast<const PointerType*>(lhs);
			auto vector = dynamic_cast<const VectorType*>(rhs);
			return Identical(pointer->UnderlyingType(), vector->ItemType());
		}
		// example: [n x TYPE] == *TYPE
		if (lhs->TypClass() == TypeClass::VECTOR && rhs->TypClass() == TypeClass::POINTER)
		{
			auto vector = dynamic_cast<const VectorType*>(lhs);
			auto pointer = dynamic_cast<const PointerType*>(rhs);
			return Identical(pointer->UnderlyingType(), vector->ItemType());
		}

		// CAUTION: should be at the very bottom
		// example: char == i8 due to char :: i8
		if (lhs->TypClass() == TypeClass::ALIAS || rhs->TypClass() == TypeClass::ALIAS)
		{
			if (auto alias = dynamic_cast<const AliasType*>(lhs))
				lhs = alias->OriginType();
			if (auto alias = dynamic_cast<const AliasType*>(rhs))
				rhs = alias->OriginType();
			return Identical(lhs, rhs);
		}

		return false;
	}

	static TypeLayout ResolveTypeSize(const Type* typ) noexcept
	{
		if (typ->TypClass() == TypeClass::POINTER)
			return POINTER_SIZE;

		if (typ->TypClass() == TypeClass::ALIAS)
			return ResolveTypeSize(typ->As<AliasType>()->OriginType());

		if (typ->TypClass() == TypeClass::VECTOR)
		{
			const VectorType* vectorTyp = typ->As<VectorType>();
			TypeLayout itemLayout = ResolveTypeSize(vectorTyp->ItemType());
			itemLayout.size *= vectorTyp->ItemCount();
			itemLayout.align *= vectorTyp->ItemCount();
			return itemLayout;
		}

		return dynamic_cast<const SizedType*>(typ)->Layout();
	}
};


} // namespace IRGenerate
