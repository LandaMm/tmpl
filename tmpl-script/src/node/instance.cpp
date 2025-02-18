
#include "include/node/instance.h"

namespace AST
{
    namespace Nodes
    {
        std::string InstanceNode::Format() const
        {
            return "Instance(" + m_target->Format() + ", " + m_fcall->Format() + ")";
        }
    }
}

