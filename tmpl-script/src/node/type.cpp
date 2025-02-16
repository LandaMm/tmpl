#include "include/node/type.h"

namespace AST::Nodes
{
    std::string TypeNode::Format() const
    {
        return "TypeNode(" + m_typename->Format() + ")";
    }
}

