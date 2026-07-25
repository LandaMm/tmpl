
#include "include/node/assign.h"

namespace AST::Nodes
{
    std::string AssignmentNode::Format() const 
    {
        return "Assign(" + m_assignee->Format() + " = " + m_value->Format() + ")";
    }
}

