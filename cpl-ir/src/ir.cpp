#include "cpl-ir/ir.h"

#include <iostream>

#include <cpl-parser/node/function.hpp>
#include <cpl-parser/node/identifier.hpp>

#include "cpl-ir/instr/call.hpp"

namespace IRGenerate
{

IR::IR(Nodes::ProgramNode* root)
	: m_rootNode(root) { }

const std::map<String, Symbol*> IR::Symbols() const noexcept { return m_symbols; }

const std::map<String, Type*> IR::Types() const noexcept { return m_localTypes; }

const std::map<String, Function*> IR::Functions() const noexcept { return m_functions; }

void IR::GenerateIR()
{
	using NT = AST::NodeType;

	/////////////////////////////////////

	AddType("void", m_arena.Alloc<IntegerType>("void", TypeLayout{0, 0}));
	AddType("i1", m_arena.Alloc<IntegerType>("i1", TypeLayout{1, 1}));
	AddType("i8", m_arena.Alloc<IntegerType>("i8", TypeLayout{8, 8}));
	AddType("i32", m_arena.Alloc<IntegerType>("i32", TypeLayout{32, 32}));
	AddType("i64", m_arena.Alloc<IntegerType>("i64", TypeLayout{64, 64}));

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
		case NT::Require:
		{
			// skip, as it is already handled by the parser
			break;
		}
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
			return nullptr;
		}

		if (symbol->SymType() != SymbolType::FUNCTION)
		{
			// TODO: better error
			assert(false && "cannot call non-callable symbol");
			return nullptr;
		}

		const FunctionType* funcSymbol = dynamic_cast<const FunctionType*>(symbol->ValueType());
		assert(funcSymbol && "should be the case due to FUNCTION symbol type");

		Array<const Value*> args;
		for (size_t i = 0; i < callNode->GetArgumentsSize(); ++i)
		{
			Node* argNode = callNode->GetArgument(i);
			args.Push(EvaluateNode(argNode));
		}

		return m_arena.Alloc<CallInstr>(symbol, std::move(args), funcSymbol->RetType());
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

[[nodiscard]] const Value* IR::EvaluateNode(Node* node)
{
	using NT = AST::NodeType;
	using LiteralType = Nodes::LiteralType;
	switch (node->GetType())
	{
	case NT::Literal:
	{
		
		auto literal = node->As<Nodes::LiteralNode>();

		switch (literal->GetLiteralType())
		{
		case LiteralType::STRING:
		{
			const String* str = literal->GetValue<String>();
			Array<Byte> data(str->Size());
			data.ResizeUninitialized(str->Size());
			std::memcpy(data.Data(), str->Data(), sizeof(char) * str->Size());
			const Type* CharType = FindType("i8");
			return m_arena.Alloc<GlobalValue>(std::move(data), m_arena.Alloc<VectorType>(CharType, str->Size()));
		}
		default:
			// TODO: better error
			assert(false && "unsupported literal type for evaluating an IR value");
			break;
		}
	}
	default:
		// TODO: better error
		assert(false && "unsupported node for evaluating an IR value");
		break;
	}

	assert(false && "UNREACHABLE");
	return nullptr;
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

	auto retType = ParseType(fn->GetReturnType());

	AddSymbol(m_arena.Alloc<Symbol>(SymbolType::FUNCTION, m_arena.Alloc<FunctionType>(symbolName, retType), origin, symbolName));

	Array<FunctionParam> params;
	for (size_t i = 0; i < fn->GetParamsSize(); ++i)
	{
		auto param = fn->GetParam(i);
		auto paramName = param->GetName()->GetName();
		const Type* paramType = ParseType(param->GetType());
		params.Emplace(FunctionParam{ paramName, paramType });
	}

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

	if (origin != SymbolOrigin::FOREIGN)
	{
		m_functions.insert({ symbolName, m_arena.Alloc<Function>(symbolName, std::move(params), block, retType) });
	}
}

void IR::GenerateVariableDeclaration(Nodes::VariableDeclaration* var)
{
	SymbolOrigin origin = SymbolOrigin::LOCAL;
	if (var->GetSymbolFlag() == Nodes::SymbolFlag::FOREIGN)
	{
		origin = SymbolOrigin::FOREIGN;
	}

	const Type* valueType = ParseType(var->GetValueType());

	AddSymbol(m_arena.Alloc<Symbol>(SymbolType::VARIABLE, valueType, origin, *var->GetName()));
}

void IR::GenerateTypeDeclaration(Nodes::TypeDeclaration* typ)
{
	String typName = typ->GetTypeName()->GetName();
	const Type* typValue = ParseType(typ->GetTypeValue());
	if (FindType(typName) != nullptr) {
		// TODO: better error
		assert(false && "redeclaration of a type");
	}
	else
	{
		AddType(typName, m_arena.Alloc<AliasType>(typName, typValue));
	}
}

const Type* IR::ParseType(Nodes::Type* typ)
{
	switch (typ->GetKind())
	{
	case Nodes::TypeKind::BASIC:
	{
		auto basicType = typ->As<Nodes::BasicType>();
		const Type* typ = FindType(basicType->GetName());
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
		const Type* underlyingType = ParseType(pointerType->GetTargetType());
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

const Type* IR::FindType(const String& name) const noexcept
{
	if (m_localTypes.find(name) == m_localTypes.end()) return nullptr;
	return m_localTypes.at(name);
}

void IR::AddSymbol(Symbol* symbol)
{
	m_symbols.insert({ symbol->Name(), symbol });
}

const Symbol* IR::FindSymbol(const String &name) const noexcept
{
	if (m_symbols.find(name) == m_symbols.end()) return nullptr;
	return m_symbols.at(name);
}

} // namespace IRGenerate

