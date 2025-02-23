
#include "include/node/loop.h"

namespace AST::Nodes
{
    std::string WhileNode::Format() const
    {
        return "WhileLoop(" + m_condition->Format() + ", " + m_body->Format() + ")";
    }

    std::string ForLoopNode::Format() const
    {
        return "ForLoop(" + m_decl->Format() + "; " + m_condition->Format() + "; " + m_loop_action->Format() + " => " + m_body->Format() + ")";
    }
}

