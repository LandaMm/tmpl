
#include "include/node/assign.h"
#include <memory>
#include "include/interpreter.h"
#include "include/node/identifier.h"

namespace Runtime
{
    void Interpreter::EvaluateAssignment(std::shared_ptr<AssignmentNode> assignment)
    {
        // TODO: support for object member reassignment
        assert(assignment->GetAssignee()->GetType() == NodeType::Identifier && "Assignee should be an identifier.");
        // TODO: support for object members name
        std::string varName = std::dynamic_pointer_cast<IdentifierNode>(assignment->GetAssignee())->GetName();

        if (!m_variables->HasItem(varName))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.UndeclaredVariable(GetFilename(), std::dynamic_pointer_cast<IdentifierNode>(assignment->GetAssignee()), "RuntimeError");
            return;
        }

        auto variable = m_variables->LookUp(varName);

        if (!variable->IsEditable())
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.ConstAssignment(GetFilename(), varName, assignment->GetLocation(), "RuntimeError");
            return;
        }

        auto newValue = Execute(assignment->GetValue());

        if (!variable->GetType()->Compare(*newValue->GetType()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.AssignMismatchType(GetFilename(), varName, newValue->GetType(), variable->GetType(), assignment->GetLocation(), "RuntimeError");
            return;
        }

        auto currValue = variable->GetValue()->Clone();

        switch (assignment->GetAssignOp())
        {
            case AST::Nodes::AssignOperator::Reassign:
                break;
            case AST::Nodes::AssignOperator::Add:
                newValue = currValue->Operate(newValue, AST::Nodes::ExpressionNode::OperatorType::PLUS);
                break;
            case AST::Nodes::AssignOperator::Subtract:
                newValue = currValue->Operate(newValue, AST::Nodes::ExpressionNode::OperatorType::MINUS);
                break;
            case AST::Nodes::AssignOperator::Multiply:
                newValue = currValue->Operate(newValue, AST::Nodes::ExpressionNode::OperatorType::MULTIPLY);
                break;
            case AST::Nodes::AssignOperator::Divide:
                newValue = currValue->Operate(newValue, AST::Nodes::ExpressionNode::OperatorType::DIVIDE);
                break;
            default:
                assert(false && "Unsupported assign operator met");
                break;
        }

        variable->SetValue(newValue);
    }
}

