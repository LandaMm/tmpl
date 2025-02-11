#include "../../include/node/generic.h"

namespace AST
{
    namespace Nodes
    {
        std::string GenericNode::Format() const
        {
            return "Generic(" + m_target->Format() + "<" + m_type->Format() + ">)";
        }
    }
}

