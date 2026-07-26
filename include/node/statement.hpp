#pragma once

#include <memory>
#include <vector>
#include "node.h"

namespace AST
{
	namespace Statements
	{
		class StatementsNode : public Node
		{
		public:
			StatementsNode(Location loc)
                : m_index(0), m_body(std::vector<std::shared_ptr<Node>>()), Node(loc) {}

		public:
			void AddItem(std::shared_ptr<Node> item) { m_body.push_back(item); }

        public:
            inline std::shared_ptr<Node> GetItem(unsigned int index) { return m_body[index]; }

        public:
            inline bool IsBlock() override { return true; }

		public:
			inline size_t GetSize() const { return m_body.size(); }

		private:
			std::vector<std::shared_ptr<Node>> m_body;
			size_t m_index;
		};

		class StatementsBody : public StatementsNode
		{
		public:
			StatementsBody(Location loc)
				: StatementsNode(loc) { }

		public:
			inline NodeType GetType() const override { return NodeType::Block; }
		};

		class IfElseStatement : public Node
		{
		public:
			IfElseStatement(std::shared_ptr<Node> condition, Location loc)
				: m_condition(condition), m_else_statement(nullptr), Node(loc)
			{
			}

		public:
			inline NodeType GetType() const override { return NodeType::IfElse; }
            inline bool IsBlock() override { return true; }

		public:
			void SetElseStatement(std::shared_ptr<Node> elseNode) { m_else_statement = elseNode; }
            void SetBody(std::shared_ptr<StatementsBody> body) { m_body = body; }
        public:
            inline std::shared_ptr<Node> GetCondition() const { return m_condition; }
            inline std::shared_ptr<Node> GetElseNode() const { return m_else_statement; }
            inline std::shared_ptr<StatementsBody> GetBody() const { return m_body; }

		private:
            std::shared_ptr<StatementsBody> m_body;
			std::shared_ptr<Node> m_condition;
			std::shared_ptr<Node> m_else_statement;
		};
    }
}

