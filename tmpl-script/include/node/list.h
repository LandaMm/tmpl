#pragma once
#ifndef LIST_H
#define LIST_H
#include"../node.h"

namespace Compiler
{
	namespace Nodes
	{
		class ListNode : public Node
		{
		private:
			std::vector<std::shared_ptr<Node>> m_items;
		public:
			ListNode() : m_items(std::vector<std::shared_ptr<Node>>()) {}
			~ListNode() {}
		public:
			inline NodeType GetType() const override { return NodeType::List; }
		private:
			std::string Format() const override { return "List"; };
		public:
			void AddItem(std::shared_ptr<Node> item);
		};
	}
}

#endif
