#ifndef PROGRAM_H
#define PROGRAM_H
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
		private:
			std::vector<std::shared_ptr<Node>> m_statements;
			size_t m_index;
		public:
			std::string Format() const override;
		public:
			ProgramNode()
                : m_index(0), m_statements(std::vector<std::shared_ptr<Node>>()),
                  Node(Location(-1, -1)) {}
			~ProgramNode() {}
		public:
			inline NodeType GetType() const override { return NodeType::Program; }
            inline std::vector<std::shared_ptr<Node>>* GetItemsPtr() { return &m_statements; }
		public:
			void AddStatement(std::shared_ptr<Node> statement);
		public:
			std::shared_ptr<Node> operator[](size_t index) { return m_statements[index]; }
        public:
            inline std::shared_ptr<Node> GetItem(unsigned int index) { return m_statements[index]; }
		public:
			inline size_t Size() { return m_statements.size(); }
		};
	}
}

#endif
