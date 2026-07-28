#pragma once

#include<string>
#include<vector>
#include<memory>
#include"../node.h"

namespace AST
{
	namespace Nodes
	{
		class ProgramNode : public Node
		{
		public:
			ProgramNode()
                : m_index(0), m_statements(std::vector<Node*>()),
                  Node(LocationSpan{Location(-1, -1), Location(-1, -1) }) {}
			~ProgramNode() {}
		public:
			inline NodeType GetType() const override { return NodeType::Program; }
            inline std::vector<Node*>* GetItemsPtr() { return &m_statements; }
		public:
			void AddStatement(Node* statement) { m_statements.push_back(statement); }
		public:
			Node* operator[](size_t index) { return m_statements[index]; }
        public:
            inline Node* GetItem(unsigned int index) { return m_statements[index]; }
		public:
			inline size_t Size() { return m_statements.size(); }
		private:
			std::vector<Node*> m_statements;
			size_t m_index;
		};
	}
}

