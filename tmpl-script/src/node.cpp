
#include"../include/node.h"

namespace AST
{
	std::ostream& operator<<(std::ostream& stream, const Node& node)
	{
		stream << node.Format();

		return stream;
	}
}
