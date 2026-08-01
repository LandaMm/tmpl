#pragma once

#include "node.h"

namespace AST
{

namespace Nodes
{

enum class SymbolFlag
{
	NONE = 0,
	FOREIGN,
	COUNT_SYMBOL_FLAGS,
}; // enum class SymbolClass

enum class SymbolType
{
	UNKNOWN = 0,
	VARIABLE,
	FUNCTION,
	COUNT_SYMBOL_TYPES,
};

class Symbol : public Node
{
public:
	Symbol(SymbolType symbolType, SymbolFlag symbolFlag, LocationSpan loc)
		: m_type(symbolType), m_flag(symbolFlag), Node(loc) { }
	~Symbol() = default;
public:
	SymbolType GetSymbolType() const noexcept { return m_type; }
	SymbolFlag GetSymbolFlag() const noexcept { return m_flag; }
private:
	SymbolType m_type;
	SymbolFlag m_flag;
}; // class Symbol

} // namespace Nodes

} // namespace AST

