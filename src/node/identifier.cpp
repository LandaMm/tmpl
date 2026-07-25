
#include"../../include/node/identifier.h"

namespace AST
{
	namespace Nodes
	{
		std::string IdentifierNode::Format() const
		{
			return "Identifier(" + m_name + ")";
		}
	}
}