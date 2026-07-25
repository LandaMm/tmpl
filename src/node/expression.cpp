
#include"../../include/node/expression.h"

namespace AST
{
	namespace Nodes
	{
        std::string ExpressionNode::Format() const
        {
            return "Expression(" + m_left->Format() + ", "
                + std::to_string((int)m_operator.GetType()) + ", "
                + m_right->Format() + ")";
        }
	}
}
