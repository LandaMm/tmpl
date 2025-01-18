

#include"../../include/node/function.h"

namespace Compiler
{
	namespace Nodes
	{
		std::string FunctionCall::Format() const
		{
			return "FunctionCall(" + std::to_string(m_args.size()) + ")";
		}
	}
}
