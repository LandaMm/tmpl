#pragma once
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
			IdentifierNode(std::string name): m_name(name) { }
			~IdentifierNode() { }
		public:
			inline NodeType GetType() const override { return NodeType::Identifier; }
		private:
			std::string Format() const override;
		public:
			inline std::string GetName() const { return m_name; }
		};
	}
}

#endif