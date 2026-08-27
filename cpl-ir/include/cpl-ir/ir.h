#pragma once

#include <map>
#include <deque>

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
#include "scope.hpp"
#include "string.hpp"

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
	const std::map<String, Symbol*>& Symbols() const noexcept;
	const std::map<String, StringLiteral>& StringLiterals() const noexcept;
	inline const std::deque<Scope*>& Scopes() const noexcept { return m_scopes; }
private:
	const Value* EvaluateNode(Node* node);
	void GenerateFunction(Nodes::FunctionDeclaration* fn);
	const Value* GenerateVariableDeclaration(Nodes::VariableDeclaration* var);
	void GenerateTypeDeclaration(Nodes::TypeDeclaration* typ);
private:
	const Type* ParseType(Nodes::Type* typ);
	String CreateStringLiteral(const String& str);
private:
	[[nodiscard]] inline Scope* CurrentScope() const noexcept { assert(!m_scopes.empty()); return m_scopes.front(); };
	void InsertScope(Scope* newScope);

	void AddSymbol(Symbol* symbol);
	const Symbol* FindSymbol(const String& name) const noexcept;
private:
	const LocalValue* FindLocal(const String& name) const noexcept;
	const Type* FindType(const String& name) const noexcept;
private:
	[[nodiscard]] inline BasicBlock* CurrentBlock() const noexcept {
		if (m_blocks.empty())
			return nullptr;
		return m_blocks.front();
	}
	inline TempValueID BlockNextTempValueId() const noexcept
	{
		// TODO: better error
		assert(CurrentBlock() && "no block is set for retrieving new temp id");
		return CurrentBlock()->NextTempValueId();
	}
	void StartBlock(BasicBlock* block);
	void PushInstr(Instr* instr);
	BasicBlock* EndBlock();
private:
	ArenaAllocator<> m_arena;
	Nodes::ProgramNode* m_rootNode;

	std::map<String, Symbol*> m_symbols;
	std::map<String, StringLiteral> m_stringLiterals;

	std::deque<Scope*> m_scopes;
	std::deque<BasicBlock*> m_blocks;

	TempValueID m_tempValueCounter = 0;
	StringLiteralId m_stringCounter = 0;
};

} // namespace IRGenerate

