#pragma once

#include <map>

#include <cpl-basics/allocator/arena.hpp>
#include <cpl-basics/array.hpp>
#include <cpl-basics/string.hpp>
#include <cpl-parser/parser.h>
#include <cpl-parser/node/function.hpp>

#include "symbol.hpp"
#include "instr.h"
#include "block.h"

#include "cplbuild.h"

namespace IRGenerate
{

using Node = AST::Node;
using namespace AST::Nodes;

class CPL_EXPORT IR
{
public:
	explicit IR(ProgramNode* root);

	IR(const IR&) = delete;
	IR& operator=(const IR&) = delete;
	IR(IR&&) = delete;
	IR& operator=(IR&&) = delete;
public:
	void GenerateIR();
private:
	[[nodiscard]] Instr* GenerateInstr(Node* node);
	void GenerateFunction(FunctionDeclaration* fn);
private:
	void AddSymbol(const Symbol& symbol);
	const Symbol* const FindSymbol(const String &name) const noexcept;
private:
	ArenaAllocator<> m_arena;
	ProgramNode* m_rootNode;
	std::map<String, Symbol> m_symbols;
	Array<NamedBlock> m_localFunctions;
};

} // namespace IRGenerate

