#ifndef STATEMENT_H
#define STATEMENT_H
#include <memory>
#include <vector>
#include "../node.h"

namespace AST
{
	namespace Statements
	{
		class StatementsNode : public Node
		{
		private:
			std::vector<std::shared_ptr<Node>> m_body;
			size_t m_index;

		public:
			StatementsNode(Location loc)
                : m_index(0), m_body(std::vector<std::shared_ptr<Node>>()), Node(loc) {}

		public:
			std::string Format() const override;

		public:
			void AddItem(std::shared_ptr<Node> item);

		public: // Iterate
			void ResetIterator() { m_index = 0; }
			std::shared_ptr<Node> Next()
            {
                if (m_index >= m_body.size()) return nullptr;
                return m_body[m_index++];
            }
			bool HasItems() { return m_index < m_body.size(); }
			size_t GetSize() { return m_body.size(); }
		};

		class IfElseStatement : public StatementsNode
		{
		private:
			std::shared_ptr<Node> m_condition;
			std::shared_ptr<Node> m_else_statement;

		public:
			inline NodeType GetType() const override { return NodeType::IfElse; }
            std::string Format() const override;

		public:
			IfElseStatement(std::shared_ptr<Node> condition, Location loc)
				: m_condition(condition), m_else_statement(nullptr), StatementsNode(loc)
			{
			}

		public:
			void SetElseStatement(std::shared_ptr<Node> elseNode) { m_else_statement = elseNode; }
		};

		class StatementsBody : public StatementsNode
		{
		public:
			inline NodeType GetType() const override { return NodeType::Block; }

		public:
			StatementsBody(Location loc)
				: StatementsNode(loc)
			{
			}
		};
	}
}

#endif
