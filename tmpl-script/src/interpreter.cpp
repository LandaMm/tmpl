
#include <cassert>
#include <memory>
#include "../include/interpreter.h"
#include "../include/error.h"
#include "include/node/function.h"
#include "include/node/statement.h"

namespace Runtime
{
	using namespace AST::Nodes;

    void Interpreter::Evaluate(std::shared_ptr<ProgramNode> program)
    {
        while (auto stmt = program->Next())
        {
            switch (stmt->GetType())
            {
                case NodeType::Require:
                    ImportModule(std::dynamic_pointer_cast<RequireMacro>(stmt));
                    break;
                case NodeType::FnDecl:
                    EvaluateFunctionDeclaration(std::dynamic_pointer_cast<FunctionDeclaration>(stmt), false);
                    break;
                case NodeType::ProcedureDecl:
                    EvaluateProcedureDeclaration(std::dynamic_pointer_cast<ProcedureDeclaration>(stmt));
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
        while (auto stmt = program->Next())
        {
            switch (stmt->GetType())
            {
                case NodeType::Require:
                    ImportModule(std::dynamic_pointer_cast<RequireMacro>(stmt));
                    break;
                case NodeType::Export:
                    EvaluateExportStatement(std::dynamic_pointer_cast<ExportStatement>(stmt));
                    break;
                case NodeType::FnDecl:
                    EvaluateFunctionDeclaration(std::dynamic_pointer_cast<FunctionDeclaration>(stmt), true);
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
			return nullptr;
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
        case NodeType::Block:
        {
            auto block = std::dynamic_pointer_cast<Statements::StatementsNode>(node);
            auto scope = std::make_shared<Environment<Variable>>(m_variables);
            m_variables = scope;
            std::shared_ptr<Value> value = nullptr;
            while (block->HasItems())
            {
                std::shared_ptr<Node> node = block->Next();
                if (node->GetType() == NodeType::Return)
                {
                    value = EvaluateReturn(std::dynamic_pointer_cast<ReturnNode>(node));
                    break;
                }
                else
                {
                    auto localVal = Execute(node);
                    if (node->IsBlock() && localVal != nullptr)
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

		return nullptr;
	}
}
