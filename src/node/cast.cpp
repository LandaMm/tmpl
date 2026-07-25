
#include "include/node/cast.h"

namespace AST::Nodes
{
    std::string CastNode::Format() const
    {
        return "CastNode(" + m_type->Format() + ", " + m_expr->Format() + ")";
    }
}

