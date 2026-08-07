#pragma once

#include <map>

#include <cpl-basics/allocator/arena.hpp>
#include <cpl-basics/array.hpp>
#include <cpl-basics/string.hpp>
#include <cpl-parser/parser.h>
#include <cpl-parser/node/function.hpp>
#include <cpl-parser/node/var_declaration.hpp>
#include <cpl-parser/node/type.hpp>

#include "type.hpp"
#include "function.hpp"
#include "symbol.hpp"
#include "value.hpp"
#include "instr.h"
#include "block.h"

#include "cplbuild.h"

namespace IRGenerate
{

using namespace AST;

class CPL_EXPORT IR
{
public:
	explicit IR(Nodes::ProgramNode* root);

	IR(const IR&) = delete;
	IR& operator=(const IR&) = delete;
	IR(IR&&) = delete;
	IR& operator=(IR&&) = delete;
public:
	void GenerateIR();
public:
	const std::map<String, Symbol*> Symbols() const noexcept;
	const std::map<String, Type*> Types() const noexcept;
	const std::map<String, Function*> Functions() const noexcept;
private:
	[[nodiscard]] Instr* GenerateInstr(Node* node);
	[[nodiscard]] const Value* EvaluateNode(Node* node);
	void GenerateFunction(Nodes::FunctionDeclaration* fn);
	void GenerateVariableDeclaration(Nodes::VariableDeclaration* var);
	void GenerateTypeDeclaration(Nodes::TypeDeclaration* typ);
private:
	const Type* ParseType(Nodes::Type* typ);
private:
	void AddType(const String& name, Type* typ);
	const Type* FindType(const String& name) const noexcept;

	void AddSymbol(Symbol* symbol);
	Symbol* FindSymbol(const String& name) const noexcept;
private:
	ArenaAllocator<> m_arena;
	Nodes::ProgramNode* m_rootNode;
	std::map<String, Symbol*> m_symbols;
	std::map<String, Type*> m_localTypes;
	std::map<String, Function*> m_functions;
};

} // namespace IRGenerate

