#include "include/node/type.h"

namespace AST::Nodes
{
    std::string TypeNode::Format() const
    {
        return "TypeNode(" + m_typename->Format() + ")";
    }

    std::string TypeDfNode::Format() const
    {
        return "TypeDf(" + m_name->Format() + " = " + m_value->Format() + ")";
    }
}

