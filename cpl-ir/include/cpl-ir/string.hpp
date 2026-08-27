#pragma once

#include <cpl-basics/def.hpp>
#include <cpl-basics/string.hpp>

namespace IRGenerate
{

using StringLiteralId = Uint32;

struct StringLiteral
{
	String data;
	StringLiteralId id;
};

} // namespace IRGenerate

