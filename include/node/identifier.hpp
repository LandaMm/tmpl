#pragma once

#include<string>
#include"../node.h"

namespace AST
{
	namespace Nodes
	{
		class IdentifierNode : public Node
		{
		public:
			IdentifierNode(std::string name, Location loc): m_name(name), Node(loc) { }
			~IdentifierNode() { }

		public:
			inline NodeType GetType() const override { return NodeType::Identifier; }
		public:
			inline std::string GetName() const { return m_name; }
		private:
			std::string m_name;
		};
	}
}

