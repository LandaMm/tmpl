#include "cpl-ir/ir.h"

#include <iostream>

#include <cpl-parser/node/function.hpp>
#include <cpl-parser/node/identifier.hpp>
#include <cpl-parser/node/assign.hpp>

#include "cpl-ir/instr/call.hpp"
#include "cpl-ir/instr/alloca.hpp"
#include "cpl-ir/instr/store.hpp"
#include "cpl-ir/instr/load.hpp"

namespace IRGenerate
{

IR::IR(Nodes::ProgramNode* root)
	: m_rootNode(root) { }

const std::map<String, Symbol*> IR::Symbols() const noexcept { return m_symbols; }

void IR::GenerateIR()
{
	using NT = AST::NodeType;

	InsertScope(m_arena.Alloc<Scope>());

	/////////////////////////////////////

	CurrentScope()->AddType("void", m_arena.Alloc<IntegerType>("void", TypeLayout{0, 1}));
	// TODO: dedicated type size for i1
	CurrentScope()->AddType("i1", m_arena.Alloc<IntegerType>("i1", TypeLayout{1, 1}));
	CurrentScope()->AddType("i8", m_arena.Alloc<IntegerType>("i8", TypeLayout{8, 8}));
	CurrentScope()->AddType("i32", m_arena.Alloc<IntegerType>("i32", TypeLayout{32, 32}));
	CurrentScope()->AddType("i64", m_arena.Alloc<IntegerType>("i64", TypeLayout{64, 64}));
	CurrentScope()->AddType("i128", m_arena.Alloc<IntegerType>("i128", TypeLayout{128, 128}));

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

	// Safe Guarding
	// 1. Make sure no pending block exists
	assert(m_blocks.empty());
}

const Value* IR::EvaluateNode(Node* node)
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
			assert(CharType);
			return m_arena.Alloc<GlobalValue>(std::move(data), m_arena.Alloc<VectorType>(CharType, str->Size()));
		}
		case LiteralType::INT:
		{
			const int* value = literal->GetValue<int>();
			// TODO: support more than 32-bits
			const Type* IntType = FindType("i32");
			assert(IntType);
			return m_arena.Alloc<ImmediateValue>(*value, IntType);
		}
		default:
			// TODO: better error
			assert(false && "unsupported literal type for evaluating an IR value");
			break;
		}

		assert(false && "UNREACHABLE");
		break;
	}
	case NT::Identifier:
	{
		auto id = node->As<Nodes::IdentifierNode>();

		if (!FindLocal(id->GetName()))
		{
			// TODO: better error
			assert(false && "referencing undefined variable");
		}

		const LocalValue* local = FindLocal(id->GetName());
		auto dst = m_arena.Alloc<TemporalValue>(BlockNextTempValueId(), local->Typ());
		PushInstr(m_arena.Alloc<LoadInstr>(dst, local, local->Typ()));
		return dst;
	}
	case NT::VarDecl:
	{
		GenerateVariableDeclaration(node->As<Nodes::VariableDeclaration>());
		return nullptr;
	}
	case NT::Assign:
	{
		auto assignmentNode = node->As<Nodes::AssignmentNode>();
		assert(assignmentNode->GetAssignOp() == Nodes::AssignOperator::Declare && "TODO: support other assignments except declarations");
		assert(assignmentNode->GetAssignee()->GetType() == NT::Identifier && "currently only variable names are supported in an assignment");
		auto target = assignmentNode->GetAssignee()->As<Nodes::IdentifierNode>();
		// declaration
		{
			if (FindLocal(target->GetName()))
			{
				// TODO: better error
				assert(false && "variable redeclaration");
			}
			const Value* value = EvaluateNode(assignmentNode->GetValue());
			auto storingValue = m_arena.Alloc<LocalValue>(target->GetName(), value->Typ());
			CurrentScope()->AddLocal(storingValue);
			PushInstr(m_arena.Alloc<AllocaInstr>(storingValue, value->Typ()));
			PushInstr(m_arena.Alloc<StoreInstr>(storingValue, value, value->Typ()));
			return storingValue;
		}
		// TODO: maybe pass evaluated value of the local instead of the local itself?
		// so it's possible to do something like that: x := y := 5 where x is expected to have a value 5
		assert(false && "UNREACHABLE");
		return nullptr;
	}
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

		if (symbol->Kind() != SymbolKind::FUNCTION)
		{
			// TODO: better error
			assert(false && "cannot call non-callable symbol");
			return nullptr;
		}

		auto funcSymbol = dynamic_cast<const Symbols::Function*>(symbol);
		assert(funcSymbol);

		const FunctionType* funcDesc = funcSymbol->FunctionDescription();
		if (callNode->GetArgumentsSize() != funcDesc->Params().Size())
		{
			// TODO: better error
			assert(false && "argument count mismatch");
			return nullptr;
		}

		Array<const Value*> args;
		for (size_t i = 0; i < callNode->GetArgumentsSize(); ++i)
		{
			const Value* argValue = EvaluateNode(callNode->GetArgument(i));
			const Type* expectedType = funcDesc->Params()[i].typ;
			if (!TypeResolver::Identical(expectedType, argValue->Typ()))
			{
				// TODO: better error
				assert(false && "argument type does not match parameter type");
				return nullptr;
			}
			args.Push(argValue);
		}

		TemporalValue* dst = m_arena.Alloc<TemporalValue>(BlockNextTempValueId(), funcDesc->RetType());
		PushInstr(m_arena.Alloc<CallInstr>(dst, symbol, std::move(args), funcDesc->RetType()));
		return dst;
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

	bool external = fn->GetSymbolFlag() == Nodes::SymbolFlag::FOREIGN;

	Node* fnName = fn->GetName();
	assert(fnName->GetType() == NT::Identifier && "Currently only simple names are supported as function name.");
	auto symbolName = fnName->As<Nodes::IdentifierNode>()->GetName();

	auto retType = ParseType(fn->GetReturnType());

	Array<FunctionType::Param> params;
	for (size_t i = 0; i < fn->GetParamsSize(); ++i)
	{
		auto param = fn->GetParam(i);
		auto paramName = param->GetName()->GetName();
		const Type* paramType = ParseType(param->GetType());
		params.Emplace(FunctionType::Param{ paramName, paramType });
	}

	const FunctionType* funcType = m_arena.Alloc<FunctionType>(symbolName, std::move(params), retType);

	AddSymbol(m_arena.Alloc<Symbols::Function>(symbolName, funcType, external ? SymbolOrigin::FOREIGN : SymbolOrigin::LOCAL));

	if (!external)
	{
		StartBlock(m_arena.Alloc<NamedBlock>(symbolName));
		auto body = fn->GetBody();
		for (size_t i = 0; i < body->GetSize(); ++i)
		{
			// TODO: we can do expression based thingy like in Rust
			EvaluateNode(body->GetItem(i));
		}
		NamedBlock* block = reinterpret_cast<NamedBlock*>(EndBlock());

		CurrentScope()->AddFunction(symbolName, m_arena.Alloc<Function>(symbolName, funcType, block));
	}
}

const Value* IR::GenerateVariableDeclaration(Nodes::VariableDeclaration* var)
{
	if (FindSymbol(*var->GetName()))
	{
		// TODO: better error
		assert(false && "variable redeclaration");
	}

	bool external = var->GetSymbolFlag() == Nodes::SymbolFlag::FOREIGN;
	const Type* expectedType = ParseType(var->GetValueType());
	const Value* initialValue = nullptr;

	if (var->HasValue())
	{
		const Value* value = EvaluateNode(var->GetValue());
		const Type* valueType = value->Typ();
		if (!TypeResolver::Identical(expectedType, valueType))
		{
			// TODO: better error
			assert(false && "type mismatch in variable declaration");
		}
		initialValue = value;
	}

	if (external)
		AddSymbol(m_arena.Alloc<Symbols::ExternalVariable>(*var->GetName(), expectedType));
	else
		AddSymbol(m_arena.Alloc<Symbols::LocalVariable>(*var->GetName(), expectedType, initialValue));

	CurrentScope()->AddLocal(m_arena.Alloc<LocalValue>(*var->GetName(), expectedType));

	// TODO: return ZeroValue or something
	return nullptr;
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
		CurrentScope()->AddType(typName, m_arena.Alloc<AliasType>(typName, typValue));
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

void IR::StartBlock(BasicBlock* block)
{
	m_blocks.push_front(block);
}

BasicBlock* IR::EndBlock()
{
	// TODO: better error
	assert(CurrentBlock() && "no block to end");
	BasicBlock* temp = CurrentBlock();
	m_blocks.pop_front();
	return temp;
}

void IR::PushInstr(Instr* instr)
{
	// TODO: better error
	assert(CurrentBlock() && "no block specified to write instructions into");
	CurrentBlock()->AddInstr(instr);
}

void IR::InsertScope(Scope* newScope)
{
	m_scopes.push_front(newScope);
}

const LocalValue* IR::FindLocal(const String& name) const noexcept
{
	for (const Scope* scope : m_scopes)
	{
		auto result = scope->FindLocal(name);
		if (result != nullptr) return result;
	}
	return nullptr;
}

const Type* IR::FindType(const String& name) const noexcept
{
	for (const Scope* scope : m_scopes)
	{
		auto result = scope->FindType(name);
		if (result != nullptr) return result;
	}
	return nullptr;
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

