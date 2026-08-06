#pragma once

#include <memory>
#include "cpl-parser/node.h"

namespace AST
{
	namespace Nodes
	{
		class Condition : public Node
		{
		public:
			enum class ConditionType
			{
				Compare,
				Less,
				Greater,
				LessEqual,
				GreaterEqual,
				NotEqual,
				None
			};
		public:
			Condition(LocationSpan loc)
                : m_left(nullptr), m_right(nullptr), m_operator(ConditionType::None), Node(loc) { }
			~Condition() { }
		public:
			inline NodeType GetType() const override { return NodeType::Condition; }
		public:
			void SetLeft(Node* left) { m_left = left; }
			void SetRight(Node* right) { m_right = right; }
			void SetOp(ConditionType op) { m_operator = op; }
		public:
			inline Node* GetLeft() { return m_left; }
			inline Node* GetRight() { return m_right; }
			inline ConditionType GetOperator() { return m_operator; }
		private:
			Node* m_left;
			Node* m_right;
			ConditionType m_operator;
		};

		class TernaryNode : public Node
		{
		public:
			inline NodeType GetType() const override { return NodeType::Ternary; }
        public:
            TernaryNode(LocationSpan loc) : Node(loc) { }
		public:
			void SetLeft(Node* left) { m_left = left; }
			void SetRight(Node* right) { m_right = right; }
			void SetCondition(Node* condition) { m_condition = condition; }
		public:
			inline Node* GetLeft() { return m_left; }
			inline Node* GetRight() { return m_right; }
			inline Node* GetCondition() { return m_condition; }
		private:
			Node* m_condition;
			Node* m_left;
			Node* m_right;
		};
	}
}

