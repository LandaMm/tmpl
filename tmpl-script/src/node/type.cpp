#include "include/node/type.h"

namespace AST::Nodes
{
    std::string TypeNode::Format() const
    {
        return "TypeNode(" + m_typename->Format() + ")";
    }

    std::string TypeTemplateNode::Format() const
    {
        return "TypeTemplateNode(" + m_typename->Format() + ")";
    }

    std::string TypeDfNode::Format() const
    {
        return "TypeDf(" + m_template->Format() + " = " + m_value->Format() + ")";
    }
}

