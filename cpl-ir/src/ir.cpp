#include "cpl-ir/ir.h"

#include <iostream>

#include <cpl-parser/node/function.hpp>
#include <cpl-parser/node/identifier.hpp>

#include "cpl-ir/instr/call.hpp"

namespace IRGenerate
{

IR::IR(Nodes::ProgramNode* root)
	: m_rootNode(root) { }

void IR::GenerateIR()
{
	using NT = AST::NodeType;

	/////////////////////////////////////

	AddType("i8", m_arena.Alloc<IntegerType>(TypeLayout {1, 1}));
	AddType("i32", m_arena.Alloc<IntegerType>(TypeLayout {4, 4}));
	AddType("i64", m_arena.Alloc<IntegerType>(TypeLayout {8, 8}));

	/////////////////////////////////////

	for (size_t i = 0; i < m_rootNode->Size(); ++i)
	{
		AST::Node* statement = m_rootNode->GetItem(i);

		switch (statement->GetType())
		{
		case NT::FnDecl:
			GenerateFunction(statement->As<Nodes::FunctionDeclaration>());
			break;
		case NT::VarDecl:
			GenerateVariableDeclaration(statement->As<Nodes::VariableDeclaration>());
			break;
		case NT::TypeDf:
			GenerateTypeDeclaration(statement->As<Nodes::TypeDeclaration>());
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
		auto callNode = node->As<Nodes::FunctionCall>();
		auto callee = callNode->GetCallee();
		assert(callee->GetType() == NT::Identifier && "Currently only function names are supported to be called.");
		auto symbol = FindSymbol(callee->As<Nodes::IdentifierNode>()->GetName());
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

void IR::GenerateFunction(Nodes::FunctionDeclaration* fn)
{
	using NT = AST::NodeType;

	SymbolOrigin origin = SymbolOrigin::LOCAL;
	if (fn->GetSymbolFlag() == Nodes::SymbolFlag::FOREIGN)
	{
		origin = SymbolOrigin::FOREIGN;
	}

	Node* fnName = fn->GetName();
	assert(fnName->GetType() == NT::Identifier && "Currently only simple names are supported as function name.");
	auto symbolName = fnName->As<Nodes::IdentifierNode>()->GetName();

	AddSymbol(m_arena.Alloc<Symbol>(SymbolType::FUNCTION, m_arena.Alloc<FunctionType>(), origin, symbolName));

	Array<FunctionParam> params;
	for (size_t i = 0; i < fn->GetParamsSize(); ++i)
	{
		auto param = fn->GetParam(i);
		auto paramName = param->GetName()->GetName();
		Type* paramType = ParseType(param->GetType());
		params.Emplace(FunctionParam{ paramName, paramType });
	}

	auto retType = ParseType(fn->GetReturnType());

	NamedBlock* block = nullptr;
	if (origin != SymbolOrigin::FOREIGN)
	{
		block = m_arena.Alloc<NamedBlock>(symbolName);
		auto body = fn->GetBody();
		for (size_t i = 0; i < body->GetSize(); ++i)
		{
			block->AddInstr(GenerateInstr(body->GetItem(i)));
		}
	}

	m_functions.insert({ symbolName, m_arena.Alloc<Function>(symbolName, std::move(params), block, retType) });
}

void IR::GenerateVariableDeclaration(Nodes::VariableDeclaration* var)
{
	SymbolOrigin origin = SymbolOrigin::LOCAL;
	if (var->GetSymbolFlag() == Nodes::SymbolFlag::FOREIGN)
	{
		origin = SymbolOrigin::FOREIGN;
	}

	Type* valueType = ParseType(var->GetValueType());

	AddSymbol(m_arena.Alloc<Symbol>(SymbolType::VARIABLE, valueType, origin, *var->GetName()));
}

void IR::GenerateTypeDeclaration(Nodes::TypeDeclaration* typ)
{
	String typName = typ->GetTypeName()->GetName();
	Type* typValue = ParseType(typ->GetTypeValue());
	AddType(typName, typValue);
}

Type* IR::ParseType(Nodes::Type* typ)
{
	switch (typ->GetKind())
	{
	case Nodes::TypeKind::BASIC:
	{
		auto basicType = typ->As<Nodes::BasicType>();
		Type* typ = FindType(basicType->GetName());
		if (!typ)
		{
			// TODO: better error
			assert(false && "use of undeclared type");
		}
		return typ;
	}
	case Nodes::TypeKind::POINTER:
	{
		auto pointerType = typ->As<Nodes::PointerType>();
		Type* underlyingType = ParseType(pointerType->GetTargetType());
		return m_arena.Alloc<PointerType>(underlyingType);
	}
	default:
	{
		// TODO: better error
		assert(false && "unsupported type kind for parsing");
		return nullptr;
	}
	}

	assert(false && "UNREACHABLE");
	return nullptr;
}

void IR::AddType(const String& name, Type* typ)
{
	m_localTypes.insert({ name, typ });
}

Type* IR::FindType(const String& name) const noexcept
{
	if (m_localTypes.find(name) == m_localTypes.end()) return nullptr;
	return m_localTypes.at(name);
}

void IR::AddSymbol(Symbol* symbol)
{
	m_symbols.insert({ symbol->Name(), symbol });
}

Symbol* IR::FindSymbol(const String &name) const noexcept
{
	if (m_symbols.find(name) == m_symbols.end()) return nullptr;
	return m_symbols.at(name);
}

} // namespace IRGenerate

