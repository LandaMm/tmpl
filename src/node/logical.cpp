

#include "../../include/node/logical.h"

namespace AST
{
    namespace Nodes
    {
        std::string Condition::Format() const
        {
            return "Condition(" + m_left->Format()
                + ", "
                + std::to_string((int)m_operator)
                + ", "
                + m_right->Format() + ")";
        }

        std::string TernaryNode::Format() const
        {
            return "Ternary(" + m_condition->Format()
                + ", " + m_left->Format() + ", " + m_right->Format() + ")";
        }
    }
}

