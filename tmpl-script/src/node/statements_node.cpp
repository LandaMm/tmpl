
#include"../../include/node/statement.h"

namespace AST
{
	namespace Statements
	{
		void StatementsNode::AddItem(std::shared_ptr<Node> item)
		{
			m_body.push_back(item);
		}
	}
}