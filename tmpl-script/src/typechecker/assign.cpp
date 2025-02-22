
#include "include/typechecker.h"

namespace Runtime
{
    void TypeChecker::HandleAssignment(std::shared_ptr<AssignmentNode> assignment)
    {
        // TODO: support for object member reassignment
        assert(assignment->GetAssignee()->GetType() == NodeType::Identifier && "Assignee should be an identifier.");
        // TODO: support for object members name
        std::string varName = std::dynamic_pointer_cast<IdentifierNode>(assignment->GetAssignee())->GetName();

        if (!m_variables->HasItem(varName))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.UndeclaredVariable(GetFilename(), std::dynamic_pointer_cast<IdentifierNode>(assignment->GetAssignee()), "TypeError");
            ReportError();
        }

        auto variable = m_variables->LookUp(varName);

        if (!variable->IsEditable())
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.ConstAssignment(GetFilename(), varName, assignment->GetLocation(), "TypeError");
            ReportError();
        }

        auto newValType = DiagnoseNode(assignment->GetValue());

        if (!variable->GetType()->Compare(*newValType))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.AssignMismatchType(GetFilename(), varName, newValType, variable->GetType(), assignment->GetLocation(), "TypeError");
            ReportError();
        }
    }
}

