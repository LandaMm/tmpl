
#include"../include/node.h"

namespace Compiler
{
	std::ostream& operator<<(std::ostream& stream, const Node& node)
	{
		stream << node.Format();

		return stream;
	}
}
