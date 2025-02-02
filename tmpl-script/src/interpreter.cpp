#include "../include/interpreter.h"
#include "../include/error.h"
#include <cassert>
#include <memory>

namespace Runtime
{
	using namespace AST::Nodes;

    void Interpreter::Evaluate(std::shared_ptr<ProgramNode> program)
    {
        while (auto stmt = program->Next())
        {
            switch (stmt->GetType())
            {
                case NodeType::FnDecl:
                    EvaluateFunctionDeclaration(std::dynamic_pointer_cast<FunctionDeclaration>(stmt));
                    break;
                case NodeType::ProcedureDecl:
                    EvaluateProcedureDeclaration(std::dynamic_pointer_cast<ProcedureDeclaration>(stmt));
                    break;
                default:
                    {
                        Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                        errorManager.RaiseError("Unsupported node type by evaluating: " + std::to_string((int)stmt->GetType()));
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
        case NodeType::Return:
            return EvaluateReturn(std::dynamic_pointer_cast<ReturnNode>(node));
        case NodeType::FunctionCall:
            return EvaluateFunctionCall(std::dynamic_pointer_cast<FunctionCall>(node));
        case NodeType::Block:
        {
            auto block = std::dynamic_pointer_cast<Statements::StatementsNode>(node);
            while (block->HasItems())
            {
                std::shared_ptr<Node> node = block->Next();
                if (node->GetType() == NodeType::Return)
                {
                    auto value = EvaluateReturn(std::dynamic_pointer_cast<ReturnNode>(node));
                    return value;
                }
                else
                {
                    Execute(node);
                }
            }
            return nullptr;
        }
		default:
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.RaiseError("Unsupported node type by executing: " + std::to_string((int)node->GetType()));
                break;
            }
		}

		return nullptr;
	}
}
