#include"../../include/node/list.h"

namespace Compiler
{
	namespace Nodes
	{
		void ListNode::AddItem(std::shared_ptr<Node> item)
		{
			m_items.push_back(item);
		}
	}
}