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
                : m_index(0), m_statements(std::vector<std::shared_ptr<Node>>()),
                  Node(Location(-1, -1)) {}
			~ProgramNode() {}
		public:
			inline NodeType GetType() const override { return NodeType::Program; }
            inline std::vector<std::shared_ptr<Node>>* GetItemsPtr() { return &m_statements; }
		public:
			void AddStatement(std::shared_ptr<Node> statement) { m_statements.push_back(statement); }
		public:
			std::shared_ptr<Node> operator[](size_t index) { return m_statements[index]; }
        public:
            inline std::shared_ptr<Node> GetItem(unsigned int index) { return m_statements[index]; }
		public:
			inline size_t Size() { return m_statements.size(); }
		private:
			std::vector<std::shared_ptr<Node>> m_statements;
			size_t m_index;
		};
	}
}

