#pragma once

#include "cpl-basics/string.hpp"
#include "cpl-parser/node.h"

namespace AST
{
	namespace Nodes
	{
		class IdentifierNode : public Node
		{
		public:
			IdentifierNode(std::string name, LocationSpan loc): m_name(name), Node(loc) { }
			~IdentifierNode() { }

		public:
			inline NodeType GetType() const override { return NodeType::Identifier; }
		public:
			inline String GetName() const { return m_name; }
		private:
			String m_name;
		};
	}
}

