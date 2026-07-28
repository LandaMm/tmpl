#pragma once

#include<memory>
#include"../node.h"

namespace AST
{

	enum class OperatorType
	{
		PLUS,
		MINUS,
		DIVIDE,
		MULTIPLY,
		NONE
	};

	class Operator
	{
	private:
		OperatorType m_type;
	public:
		Operator(OperatorType type) : m_type(type) {}
		~Operator() {}
	public:
		inline OperatorType GetType() const { return m_type; }
	};

	namespace Nodes
	{
		class ExpressionNode : public Node
		{
		public:
			ExpressionNode(Node* left, Node* right, Operator oper, LocationSpan loc)
				: m_left(left), m_right(right), m_operator(oper), Node(loc) {}
			ExpressionNode(LocationSpan loc)
				: m_left(nullptr), m_right(nullptr), m_operator(OperatorType::NONE), Node(loc) {}
			~ExpressionNode() {}
		public:
			inline NodeType GetType() const override { return NodeType::Expression; };
		public:
			inline Node* GetLeft() const { return m_left; }
			inline Node* GetRight() const { return m_right; }
			inline Operator GetOperator() const { return m_operator; }
			void SetRight(Node* right) { m_right = right; }
			void SetLeft(Node* left) { m_left = left; }
			void SetOperator(Operator op) { m_operator = op; }
		private:
			Node* m_left;
			Operator m_operator;
			Node* m_right;
		};
	}
}

