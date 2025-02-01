
#include "../../include/node/object_member.h"

namespace AST
{
    namespace Nodes
    {
        std::string ObjectMember::Format() const
        {
            return "ObjectMember(" + m_member->Format() + " of " + m_obj->Format() + ")";
        }
    }
}

