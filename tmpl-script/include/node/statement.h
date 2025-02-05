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

        public:
            inline bool IsBlock() override { return true; }

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

		class IfElseStatement : public Node
		{
		private:
            std::shared_ptr<StatementsBody> m_body;
			std::shared_ptr<Node> m_condition;
			std::shared_ptr<Node> m_else_statement;

		public:
			inline NodeType GetType() const override { return NodeType::IfElse; }
            std::string Format() const override;
            inline bool IsBlock() override { return true; }

		public:
			IfElseStatement(std::shared_ptr<Node> condition, Location loc)
				: m_condition(condition), m_else_statement(nullptr), Node(loc)
			{
			}

		public:
			void SetElseStatement(std::shared_ptr<Node> elseNode) { m_else_statement = elseNode; }
            void SetBody(std::shared_ptr<StatementsBody> body) { m_body = body; }
        public:
            inline std::shared_ptr<Node> GetCondition() const { return m_condition; }
            inline std::shared_ptr<Node> GetElseNode() const { return m_else_statement; }
            inline std::shared_ptr<StatementsBody> GetBody() const { return m_body; }
		};
    }
}

#endif
