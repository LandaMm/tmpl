
#include "include/node/loop.h"

namespace AST::Nodes
{
    std::string WhileNode::Format() const
    {
        return "WhileLoop(" + m_condition->Format() + ", " + m_body->Format() + ")";
    }
}

