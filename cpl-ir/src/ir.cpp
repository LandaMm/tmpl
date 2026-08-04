#include "ir.h"

#include <iostream>

#include <cpl-parser/node/function.hpp>
#include <cpl-parser/node/identifier.hpp>

#include "instr/call.hpp"

namespace IRGenerate
{

IR::IR(ProgramNode* root)
	: m_rootNode(root) { }

void IR::GenerateIR()
{
	using NT = AST::NodeType;
	for (size_t i = 0; i < m_rootNode->Size(); ++i)
	{
		AST::Node* statement = m_rootNode->GetItem(i);

		switch (statement->GetType())
		{
		case NT::FnDecl:
			GenerateFunction(statement->As<FunctionDeclaration>());
			break;
		default:
		{
			// TODO: better error
			std::cerr << "Unsupported statement type for IR generation: " << static_cast<int>(statement->GetType()) << std::endl;
			assert(false);
		}
		}
	}
}

[[nodiscard]] Instr* IR::GenerateInstr(Node* node)
{
	using NT = AST::NodeType;
	switch (node->GetType())
	{
	case NT::FunctionCall:
	{
		auto callNode = node->As<FunctionCall>();
		auto callee = callNode->GetCallee();
		assert(callee->GetType() == NT::Identifier && "Currently only function names are supported to be called.");
		auto symbol = FindSymbol(callee->As<IdentifierNode>()->GetName());
		if (!symbol)
		{
			// TODO: better error
			assert(false && "calling an undefined symbol");
		}
		return m_arena.Alloc<CallInstr>(*symbol);
	}
	default:
	{
		// TODO: better error
		std::cerr << "Unsupported node type for IR generation: " << static_cast<int>(node->GetType()) << std::endl;
		assert(false);
		return nullptr;
	}
	}

	assert(false && "UNREACHABLE");
}

void IR::GenerateFunction(FunctionDeclaration* fn)
{
	using NT = AST::NodeType;

	SymbolOrigin origin = SymbolOrigin::LOCAL;
	if (fn->GetSymbolFlag() == SymbolFlag::FOREIGN)
	{
		origin = SymbolOrigin::FOREIGN;
	}

	Node* fnName = fn->GetName();
	assert(fnName->GetType() == NT::Identifier && "Currently only simple names are supported as function name.");
	auto symbolName = fnName->As<IdentifierNode>()->GetName();

	Symbol symbol(SymbolType::FUNCTION, origin, symbolName);
	AddSymbol(symbol);

	if (fn->GetSymbolFlag() == SymbolFlag::FOREIGN)
	{
		return;
	}

	NamedBlock block(symbolName);
	auto body = fn->GetBody();
	for (size_t i = 0; i < body->GetSize(); ++i)
	{
		block.AddInstr(GenerateInstr(body->GetItem(i)));
	}

	m_localFunctions.Emplace(std::move(block));
}

void IR::AddSymbol(const Symbol& symbol)
{
	m_symbols.insert({ symbol.Name(), symbol });
}

const Symbol* const IR::FindSymbol(const String &name) const noexcept
{
	if (m_symbols.find(name) == m_symbols.end()) return nullptr;
	return &m_symbols.at(name);
}

} // namespace IRGenerate

