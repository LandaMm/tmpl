
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

        variable->SetValue(newValue);
    }
}

