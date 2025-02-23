
#include "include/node/assign.h"
#include "include/node/instance.h"
#include "include/node/loop.h"
#include <cassert>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include "../include/interpreter.h"
#include "../include/error.h"
#include "../include/interpreter/value.h"
#include "../include/node/function.h"
#include "../include/node/statement.h"
#include "../include/iterator.h"

namespace Runtime
{
	using namespace AST::Nodes;

    Interpreter::~Interpreter()
    {
        for (auto it = m_handles->Begin(); it != m_handles->End(); it++)
        {
            CloseHandle(it->first);
        }
    }

    void Interpreter::CloseHandle(std::string handleKey)
    {
        auto handle = m_handles->LookUp(handleKey);
#ifdef _WIN32
        FreeLibrary((HMODULE)*handle);
#else
        dlclose(*handle);
#endif
    }

    void Interpreter::Evaluate(std::shared_ptr<ProgramNode> program)
    {
        auto it = std::make_shared<Common::Iterator>(program->Size());
        while (it->HasItems())
        {
            auto stmt = program->GetItem(it->GetPosition());
            it->Next();
            switch (stmt->GetType())
            {
                case NodeType::Require:
                    ImportModule(std::dynamic_pointer_cast<RequireMacro>(stmt));
                    break;
                case NodeType::Extern:
                {
                    auto ext = std::dynamic_pointer_cast<ExternMacro>(stmt);
                    EvaluateFunctionDeclaration(ext->GetFnSignature(), false, true);
                    break;
                }
                case NodeType::FnDecl:
                    EvaluateFunctionDeclaration(std::dynamic_pointer_cast<FunctionDeclaration>(stmt), false, false);
                    break;
                case NodeType::ProcedureDecl:
                    EvaluateProcedureDeclaration(std::dynamic_pointer_cast<ProcedureDeclaration>(stmt));
                    break;
                case NodeType::TypeDf:
                    EvaluateTypeDefinition(std::dynamic_pointer_cast<TypeDfNode>(stmt), false);
                    break;
                case NodeType::Export:
                    // Ignore export in the tmpl executable
                    break;
                default:
                    {
                        Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                        errorManager.RaiseError("Unsupported node type by evaluating: " + std::to_string((int)stmt->GetType()), "RuntimeError");
                        break;
                    }
            }
        }
    }

    void Interpreter::EvaluateModule(std::shared_ptr<ProgramNode> program)
    {
        auto it = std::make_shared<Common::Iterator>(program->Size());
        while (it->HasItems())
        {
            auto stmt = program->GetItem(it->GetPosition());
            it->Next();
            switch (stmt->GetType())
            {
                case NodeType::Require:
                    ImportModule(std::dynamic_pointer_cast<RequireMacro>(stmt));
                    break;
                case NodeType::Export:
                    EvaluateExportStatement(std::dynamic_pointer_cast<ExportStatement>(stmt));
                    break;
                case NodeType::Extern:
                {
                    auto ext = std::dynamic_pointer_cast<ExternMacro>(stmt);
                    EvaluateFunctionDeclaration(ext->GetFnSignature(), false, true);
                    break;
                }
                case NodeType::FnDecl:
                    EvaluateFunctionDeclaration(std::dynamic_pointer_cast<FunctionDeclaration>(stmt), true, false);
                    break;
                case NodeType::TypeDf:
                    EvaluateTypeDefinition(std::dynamic_pointer_cast<TypeDfNode>(stmt), false);
                    break;
                default:
                    {
                        Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                        errorManager.RaiseError("Unsupported node type by evaluating imported module: " + std::to_string((int)stmt->GetType()), "RuntimeError");
                        break;
                    }
            }
        }
    }

	std::shared_ptr<Value> Interpreter::Execute(std::shared_ptr<Node> node)
	{
		switch (node->GetType())
		{
		case NodeType::Expression:
			return EvaluateExpression(std::dynamic_pointer_cast<ExpressionNode>(node));
		case NodeType::Literal:
			return EvaluateLiteral(std::dynamic_pointer_cast<LiteralNode>(node));
		case NodeType::VarDecl:
			EvaluateVariableDeclaration(std::dynamic_pointer_cast<VarDeclaration>(node));
			return std::make_shared<VoidValue>();
		case NodeType::Identifier:
			return EvaluateIdentifier(std::dynamic_pointer_cast<IdentifierNode>(node));
		case NodeType::Condition:
			return EvaluateCondition(std::dynamic_pointer_cast<Condition>(node));
		case NodeType::Ternary:
			return EvaluateTernary(std::dynamic_pointer_cast<TernaryNode>(node));
		case NodeType::Unary:
			return EvaluateUnary(std::dynamic_pointer_cast<UnaryNode>(node));
        case NodeType::Return:
            return EvaluateReturn(std::dynamic_pointer_cast<ReturnNode>(node));
        case NodeType::FunctionCall:
            return EvaluateFunctionCall(std::dynamic_pointer_cast<FunctionCall>(node));
        case NodeType::IfElse:
            return EvaluateIfElseStatement(std::dynamic_pointer_cast<Statements::IfElseStatement>(node));
        case NodeType::While:
            return EvaluateWhileLoop(std::dynamic_pointer_cast<WhileNode>(node));
        case NodeType::For:
            return EvaluateForLoop(std::dynamic_pointer_cast<ForLoopNode>(node));
        case NodeType::Instance:
            return EvaluateInstance(std::dynamic_pointer_cast<InstanceNode>(node));
        case NodeType::Cast:
            return EvaluateTypeCasting(std::dynamic_pointer_cast<CastNode>(node));
        case NodeType::Break:
            throw BreakException();
        case NodeType::Assign:
        {
            EvaluateAssignment(std::dynamic_pointer_cast<AssignmentNode>(node));
            break;
        }
        case NodeType::Block:
        {
            auto block = std::dynamic_pointer_cast<Statements::StatementsNode>(node);
            auto scope = std::make_shared<Environment<Variable>>(m_variables);
            m_variables = scope;
            std::shared_ptr<Value> value = std::make_shared<VoidValue>();
            auto it = std::make_shared<Common::Iterator>(block->GetSize());
            while (it->HasItems())
            {
                auto node = block->GetItem(it->GetPosition());
                it->Next();
                if (node->GetType() == NodeType::Return)
                {
                    value = EvaluateReturn(std::dynamic_pointer_cast<ReturnNode>(node));
                    break;
                }
                else
                {
                    auto localVal = Execute(node);
                    if (node->IsBlock() && !localVal->GetType()->Compare(ValType("void")))
                    {
                        value = localVal;
                        break;
                    }
                }
            }
            assert(scope->GetParent() != nullptr && "Scope parent gone.");
            m_variables = scope->GetParent();
            assert(m_variables != scope && "Parent and child are located at the same memory space.");
            return value;
        }
		default:
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.RaiseError("Unsupported node type by executing: " + std::to_string((int)node->GetType()), "RuntimeError");
                break;
            }
		}

		return std::make_shared<VoidValue>();
	}
}
