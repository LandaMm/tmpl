#include "cpl-ir/type.hpp"

namespace IRGenerate
{

bool operator==(const Type& lhs, const Type& rhs) noexcept
{
		if (static_cast<int>(lhs.TypClass()) == static_cast<int>(rhs.TypClass()))
		{
			return true;
		}

		if (lhs.TypClass() == TypeClass::VECTOR && rhs.TypClass() == TypeClass::POINTER)
		{
			const VectorType* vectorType   = dynamic_cast<const VectorType*>(&lhs);
			const PointerType* pointerType = dynamic_cast<const PointerType*>(&rhs);
			return operator==(*vectorType->ItemType(), *pointerType->UnderlyingType());
		}

		if (lhs.TypClass() == TypeClass::POINTER && rhs.TypClass() == TypeClass::VECTOR)
		{
			const VectorType* vectorType   = dynamic_cast<const VectorType*>(&rhs);
			const PointerType* pointerType = dynamic_cast<const PointerType*>(&lhs);
			return operator==(*vectorType->ItemType(), *pointerType->UnderlyingType());
		}

		return false;
	}

}

