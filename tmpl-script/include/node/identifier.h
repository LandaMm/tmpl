#ifndef IDENTIFIER_H
#define IDENTIFIER_H
#include<string>
#include"../node.h"

namespace AST
{
	namespace Nodes
	{
		class IdentifierNode : public Node
		{
		private:
			std::string m_name;
		public:
			IdentifierNode(std::string name, Location loc): m_name(name), Node(loc) { }
			~IdentifierNode() { }
		public:
			inline NodeType GetType() const override { return NodeType::Identifier; }
		public:
			std::string Format() const override;
		public:
			inline std::string GetName() const { return m_name; }
		};
	}
}

#endif
