#include "cpl-ir/ir.h"

#include <format>
#include <iostream>

#include <cpl-parser/node/function.hpp>
#include <cpl-parser/node/identifier.hpp>
#include <cpl-parser/node/assign.hpp>
#include <cpl-parser/node/unary.hpp>
#include <cpl-parser/node/return.hpp>
#include <cpl-parser/node/loop.hpp>

#include "cpl-ir/instr/call.hpp"
#include "cpl-ir/instr/alloca.hpp"
#include "cpl-ir/instr/store.hpp"
#include "cpl-ir/instr/load.hpp"
#include "cpl-ir/instr/ptr.hpp"
#include "cpl-ir/instr/arithmetic.hpp"
#include "cpl-ir/instr/return.hpp"
#include "cpl-ir/instr/branch.hpp"

namespace IRGenerate
{

IR::IR(Nodes::ProgramNode* root)
	: m_rootNode(root) { }

#if 0
const std::map<String, Symbol*>& IR::Symbols() const noexcept { return m_symbols; }
#endif

const std::map<String, StringLiteral>& IR::StringLiterals() const noexcept { return m_stringLiterals; }

void IR::GenerateIR()
{
	using NT = AST::NodeType;

	Scope* globalScope = m_arena.Alloc<Scope>();
	InsertScope(globalScope);

	/////////////////////////////////////

	CurrentScope()->AddType("void", m_arena.Alloc<IntegerType>("void", TypeLayout{ {}, {} }));
	// TODO: dedicated type size for i1
	CurrentScope()->AddType("i1", m_arena.Alloc<IntegerType>("i1", TypeLayout{MemSize::FromBits(1), MemSize::FromBits(1)}));
	CurrentScope()->AddType("i8", m_arena.Alloc<IntegerType>("i8", TypeLayout{MemSize::FromBits(8), MemSize::FromBits(8)}));
	CurrentScope()->AddType("i32", m_arena.Alloc<IntegerType>("i32", TypeLayout{MemSize::FromBits(32), MemSize::FromBits(32)}));
	CurrentScope()->AddType("i64", m_arena.Alloc<IntegerType>("i64", TypeLayout{MemSize::FromBits(64), MemSize::FromBits(64)}));
	CurrentScope()->AddType("i128", m_arena.Alloc<IntegerType>("i128", TypeLayout{MemSize::FromBits(128), MemSize::FromBits(128)}));

	/////////////////////////////////////

	// StartBlock(m_arena.Alloc<BasicBlock>());

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

	// EndBlock();

	// Safe Guarding
	// 1. Make sure no pending block exists
	assert(m_currentFn == nullptr);
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
			// TODO: think of better way of finding and resolving item type of string
			const Type* charType = FindType("i8");
			const Type* ptrType = m_arena.Alloc<PointerType>(charType);
			auto dst = m_arena.Alloc<TemporalValue>(BlockNextTempValueId(), ptrType);
			PushInstr(m_arena.Alloc<PtrInstr>(dst, m_arena.Alloc<GlobalValue>(CreateStringLiteral(*str), ptrType), ptrType));
			return dst;
		}
		case LiteralType::INT:
		{
			const int* value = literal->GetValue<int>();
			// TODO: support more than 32-bits
			const Type* IntType = FindType("i32");
			assert(IntType);
			return m_arena.Alloc<ImmediateValue>(*value, IntType);
		}
		case LiteralType::BOOL:
		{
			const bool* value = literal->GetValue<bool>();
			// TODO: support more than 32-bits
			const Type* BoolType = FindType("i8");
			assert(BoolType);
			return m_arena.Alloc<ImmediateValue>(static_cast<Int64>(*value), BoolType);
		}
		default:
			// TODO: better error
			assert(false && "unsupported literal type for evaluating an IR value");
			break;
		}

		assert(false && "UNREACHABLE");
		break;
	}
	case NT::Unary:
	{
		auto unary = node->As<Nodes::UnaryNode>();
		
		const Value* target = EvaluateNode(unary->GetTarget());
		auto dst = m_arena.Alloc<TemporalValue>(BlockNextTempValueId(), target->Typ());

		using UOp = Nodes::UnaryOperator;
		switch (unary->GetOperator())
		{
		case UOp::Negative:
			PushInstr(m_arena.Alloc<NegInstr>(dst, target));
			break;
		default:
			// TODO: better error
			assert(false && "unsupported unary operator");
			break;
		}

		return dst;
	}
	break;
	case NT::Return:
	{
		auto retNode = node->As<Nodes::ReturnNode>();

		const Value* retValue = EvaluateNode(retNode->GetValue());

		PushInstr(m_arena.Alloc<RetInstr>(retValue));

		return retValue;
	}
	break;
	case NT::Identifier:
	{
		auto id = node->As<Nodes::IdentifierNode>();

		const LocalValue* local = FindLocal(id->GetName());
		const Symbol* symbol = FindSymbol(id->GetName());
		if (!local && !symbol)
		{
			// TODO: better error
			assert(false && "referencing undefined variable/function");
		}

		if (local)
		{
			auto dst = m_arena.Alloc<TemporalValue>(BlockNextTempValueId(), local->Typ());
			PushInstr(m_arena.Alloc<LoadInstr>(dst, local, local->Typ()));
			return dst;
		}
		else if (symbol)
		{
			if (auto localVariable = dynamic_cast<const Symbols::LocalVariable*>(symbol))
			{
				auto dst = m_arena.Alloc<TemporalValue>(BlockNextTempValueId(), localVariable->Typ());
				PushInstr(m_arena.Alloc<LoadInstr>(dst, m_arena.Alloc<GlobalValue>(localVariable->Name(), localVariable->Typ()), localVariable->Typ()));
				return dst;
			}
			else
			{
				assert(false && "unsupported symbol kind for loading");
			}
		}

		assert(false && "UNREACHABLE");
		break;
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
		// so it's possible to do something like that: x := y := 5 where x is expected to have a value 5 directly and not through accessing variable a
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
	case NT::While:
	{
		auto loop = node->As<Nodes::WhileNode>();
		assert(loop);
		assert(CurrentFn());

		auto fn = CurrentFn();

		// Condition block
		BasicBlock* condBlock = fn->CreateBlock(m_arena);

		PushInstr(m_arena.Alloc<JmpInstr>(condBlock->Id()));
		fn->PushBlock(condBlock);

		BasicBlock* bodyBlock = fn->CreateBlock(m_arena);
		BasicBlock* endBlock = fn->CreateBlock(m_arena);

		const Value* condition = EvaluateNode(loop->GetCondition());
		PushInstr(m_arena.Alloc<BranchInstr>(bodyBlock->Id(), endBlock->Id(), condition));
		fn->PushBlock(bodyBlock);

		GenerateStatements(loop->GetBody());

		PushInstr(m_arena.Alloc<JmpInstr>(condBlock->Id()));
		fn->PushBlock(endBlock);

		return nullptr;
	}
	break;
	case NT::Expression:
		return EvaluateExpression(node->As<Nodes::ExpressionNode>());
	default:
		// TODO: better error
		assert(false && "unsupported node for evaluating an IR value");
		break;
	}

	assert(false && "UNREACHABLE");
	return nullptr;
}

const Value* IR::EvaluateExpression(Nodes::ExpressionNode* expr)
{
	const Value* left = EvaluateNode(expr->GetLeft());
	const Value* right = EvaluateNode(expr->GetRight());

	if (!TypeResolver::Identical(left->Typ(), right->Typ()))
	{
		// TODO: better error
		assert(false && "left and right sides of an expression differ in type");
		return nullptr;
	}

	TemporalValue* dst = m_arena.Alloc<TemporalValue>(BlockNextTempValueId(), left->Typ());

	switch (expr->GetOperator().GetType())
	{
	case OperatorType::PLUS:
		PushInstr(m_arena.Alloc<AddInstr>(dst, left, right));
		return dst;
	case OperatorType::MINUS:
		PushInstr(m_arena.Alloc<SubInstr>(dst, left, right));
		return dst;
	case OperatorType::MULTIPLY:
		PushInstr(m_arena.Alloc<MulInstr>(dst, left, right));
		return dst;
	case OperatorType::DIVIDE:
		PushInstr(m_arena.Alloc<DivInstr>(dst, left, right));
		return dst;
	case OperatorType::NONE:
	default:
		// TODO: better error
		assert(false && "unsupported expression operator has been used");
	}

	assert(false && "UNREACHABLE");
	return nullptr;
}

void IR::GenerateStatements(Statements::StatementsBody* body)
{
	for (size_t i = 0; i < body->GetSize(); ++i)
	{
		// TODO: we can do expression based thingy like in Rust
		EvaluateNode(body->GetItem(i));
	}
}

void IR::GenerateFunction(Nodes::FunctionDeclaration* fnNode)
{
	using NT = AST::NodeType;

	bool external = fnNode->GetSymbolFlag() == Nodes::SymbolFlag::FOREIGN;

	Node* fnName = fnNode->GetName();
	assert(fnName->GetType() == NT::Identifier && "Currently only simple names are supported as function name.");
	auto symbolName = fnName->As<Nodes::IdentifierNode>()->GetName();

	auto retType = ParseType(fnNode->GetReturnType());

	Array<FunctionType::Param> paramTypes;
	Array<FunctionParam*> fnParams;
	for (size_t i = 0; i < fnNode->GetParamsSize(); ++i)
	{
		auto param = fnNode->GetParam(i);
		auto paramName = param->GetName()->GetName();
		const Type* paramType = ParseType(param->GetType());
		paramTypes.Emplace(FunctionType::Param{ paramName, paramType });
		fnParams.Emplace(m_arena.Alloc<FunctionParam>(m_arena.Alloc<LocalValue>(paramName, paramType), std::nullopt));
	}

	const FunctionType* funcType = m_arena.Alloc<FunctionType>(symbolName, std::move(paramTypes), retType);

	CurrentScope()->AddSymbol(m_arena.Alloc<Symbols::Function>(symbolName, funcType, external ? SymbolOrigin::FOREIGN : SymbolOrigin::LOCAL));

	if (!external)
	{
		Scope* fnScope = m_arena.Alloc<Scope>();

		for (const auto& fnParam : fnParams)
		{
			fnScope->AddLocal(fnParam->destination);
		}

		InsertScope(fnScope);

		Function* fn = m_arena.Alloc<Function>(symbolName, std::move(fnParams), funcType, fnScope);

		fn->PushBlock(fn->CreateBlock(m_arena));

		StartFunction(fn);
		auto body = fnNode->GetBody();
		GenerateStatements(body);

		assert(fn == EndFunction() && "ENCOUNTERED AN UNEXPECTED FUNCTION");
		assert(fnScope == EndScope() && "ENCOUNTERED AN UNEXPECTED SCOPE");

		CurrentScope()->AddFunction(symbolName, fn);
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
		CurrentScope()->AddSymbol(m_arena.Alloc<Symbols::ExternalVariable>(*var->GetName(), expectedType));
	else
		CurrentScope()->AddSymbol(m_arena.Alloc<Symbols::LocalVariable>(*var->GetName(), expectedType, initialValue));

	// CurrentScope()->AddLocal(m_arena.Alloc<LocalValue>(*var->GetName(), expectedType));

	return m_arena.Alloc<GlobalValue>(*var->GetName(), expectedType);
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

String IR::CreateStringLiteral(const String& str)
{
	if (m_stringLiterals.find(str) != m_stringLiterals.end()) return std::format("@str.{}", m_stringLiterals.at(str).id);

	StringLiteralId stringId = m_stringCounter++;

	m_stringLiterals.insert({ str, StringLiteral{str, stringId} });

	return std::format("@str.{}", stringId);
}

#if 0
void IR::StartBlock(BasicBlock* block)
{
	m_blocks.push_front(block);
}
#endif

void IR::StartFunction(Function* fn)
{
	m_currentFn = fn;
}

#if 0
BasicBlock* IR::EndBlock()
{
	// TODO: better error
	assert(CurrentBlock() && "no block to end");
	BasicBlock* temp = CurrentBlock();
	m_blocks.pop_front();
	return temp;
}
#endif

Function* IR::EndFunction()
{
	// TODO: better error
	assert(CurrentFn() && "no function to end");
	Function* temp = CurrentFn();
	m_currentFn = nullptr;
	return temp;
}

void IR::PushInstr(Instr* instr)
{
	// TODO: better error
	assert(CurrentFn() && "no function specified to write instructions into");
	assert(CurrentFn()->CurrentBlock() && "no block specified to write instructions into");
	CurrentFn()->CurrentBlock()->AddInstr(instr);
}

void IR::InsertScope(Scope* newScope)
{
	m_scopes.push_front(newScope);
}

Scope* IR::EndScope()
{
	Scope* scope = m_scopes.front();
	m_scopes.pop_front();
	return scope;
}

const Symbol* IR::FindSymbol(const String& name) const noexcept
{
	for (const Scope* scope : m_scopes)
	{
		auto result = scope->FindSymbol(name);
		if (result != nullptr) return result;
	}
	return nullptr;
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

#if 0
void IR::AddSymbol(Symbol* symbol)
{
	m_symbols.insert({ symbol->Name(), symbol });
}

const Symbol* IR::FindSymbol(const String &name) const noexcept
{
	if (m_symbols.find(name) == m_symbols.end()) return nullptr;
	return m_symbols.at(name);
}
#endif

} // namespace IRGenerate

