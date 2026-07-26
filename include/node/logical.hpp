#pragma once

#include <memory>
#include "node.h"

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
			void SetLeft(std::shared_ptr<Node> left) { m_left = left; }
			void SetRight(std::shared_ptr<Node> right) { m_right = right; }
			void SetOp(ConditionType op) { m_operator = op; }
		public:
			inline std::shared_ptr<Node> GetLeft() { return m_left; }
			inline std::shared_ptr<Node> GetRight() { return m_right; }
			inline ConditionType GetOperator() { return m_operator; }
		private:
			std::shared_ptr<Node> m_left;
			std::shared_ptr<Node> m_right;
			ConditionType m_operator;
		};

		class TernaryNode : public Node
		{
		public:
			inline NodeType GetType() const override { return NodeType::Ternary; }
        public:
            TernaryNode(LocationSpan loc) : Node(loc) { }
		public:
			void SetLeft(std::shared_ptr<Node> left) { m_left = left; }
			void SetRight(std::shared_ptr<Node> right) { m_right = right; }
			void SetCondition(std::shared_ptr<Node> condition) { m_condition = condition; }
		public:
			inline std::shared_ptr<Node> GetLeft() { return m_left; }
			inline std::shared_ptr<Node> GetRight() { return m_right; }
			inline std::shared_ptr<Node> GetCondition() { return m_condition; }
		private:
			std::shared_ptr<Node> m_condition;
			std::shared_ptr<Node> m_left;
			std::shared_ptr<Node> m_right;
		};
	}
}

