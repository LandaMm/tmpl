
#include"../../include/node/identifier.h"

namespace Compiler
{
	namespace Nodes
	{
		std::string IdentifierNode::Format() const
		{
			return "Identifier(" + m_name + ")";
		}
	}
}