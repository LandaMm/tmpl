#pragma once

#include <memory>
#include <vector>
#include "cpl-parser/node.h"

namespace AST
{
	namespace Statements
	{
		class StatementsNode : public Node
		{
		public:
			StatementsNode(LocationSpan loc)
                : m_index(0), m_body(std::vector<Node*>()), Node(loc) {}

		public:
			void AddItem(Node* item) { m_body.push_back(item); }

        public:
            inline Node* GetItem(unsigned int index) { return m_body[index]; }

        public:
            inline bool IsBlock() override { return true; }

		public:
			inline size_t GetSize() const { return m_body.size(); }

		private:
			std::vector<Node*> m_body;
			size_t m_index;
		};

		class StatementsBody : public StatementsNode
		{
		public:
			StatementsBody(LocationSpan loc)
				: StatementsNode(loc) { }

		public:
			inline NodeType GetType() const override { return NodeType::Block; }
		};

		class IfElseStatement : public Node
		{
		public:
			IfElseStatement(Node* condition, LocationSpan loc)
				: m_condition(condition), m_else_statement(nullptr), Node(loc)
			{
			}

		public:
			inline NodeType GetType() const override { return NodeType::IfElse; }
            inline bool IsBlock() override { return true; }

		public:
			void SetElseStatement(Node* elseNode) { m_else_statement = elseNode; }
            void SetBody(StatementsBody* body) { m_body = body; }
        public:
            inline Node* GetCondition() const { return m_condition; }
            inline Node* GetElseNode() const { return m_else_statement; }
            inline StatementsBody* GetBody() const { return m_body; }

		private:
            StatementsBody* m_body;
			Node* m_condition;
			Node* m_else_statement;
		};
    }
}

