#pragma once

#ifndef EXPRESSION_H
#define EXPRESSION_H
#include<memory>
#include"../node.h"

namespace AST
{
	namespace Nodes
	{
		class ExpressionNode : public Node
		{
		public:
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
		private:
			std::shared_ptr<Node> m_left;
			Operator m_operator;
			std::shared_ptr<Node> m_right;
		public:
			inline NodeType GetType() const override { return NodeType::Expression; };
		private:
			// TODO:
			std::string Format() const override { return "Expression()"; }
		public:
			ExpressionNode(std::shared_ptr<Node> left, std::shared_ptr<Node> right, Operator oper) : m_left(left), m_right(right), m_operator(oper) {}
			ExpressionNode() :m_left(nullptr), m_right(nullptr), m_operator(OperatorType::NONE) {}
			~ExpressionNode() {}
		public:
			inline std::shared_ptr<Node> GetLeft() const { return m_left; }
			inline std::shared_ptr<Node> GetRight() const { return m_right; }
			inline Operator GetOperator() const { return m_operator; }
			void SetRight(std::shared_ptr<Node> right) { m_right = right; }
			void SetLeft(std::shared_ptr<Node> left) { m_left = left; }
			void SetOperator(Operator op) { m_operator = op; }
		};
	}
}

#endif
