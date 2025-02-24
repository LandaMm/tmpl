#ifndef LIST_H
#define LIST_H
#include<vector>
#include<memory>
#include"../node.h"
#include "include/node/type.h"

namespace AST
{
	namespace Nodes
	{
		class ListNode : public Node
		{
		private:
            std::shared_ptr<TypeNode> m_items_type;
			std::vector<std::shared_ptr<Node>> m_items;
		public:
			ListNode(std::shared_ptr<TypeNode> itemsType, Location loc)
                : m_items_type(itemsType), m_items(std::vector<std::shared_ptr<Node>>()),
                  Node(loc) {}
			~ListNode() {}
		public:
			inline NodeType GetType() const override { return NodeType::List; }
		public:
			std::string Format() const override { return "List"; };
		public:
			void AddItem(std::shared_ptr<Node> item);
        public:
            inline unsigned int Size() const { return m_items.size(); }
            inline std::shared_ptr<Node> GetItem(unsigned int index) 
                const { return m_items[index]; }
            inline std::shared_ptr<TypeNode> GetItemsType() const { return m_items_type; }
		};
	}
}

#endif
