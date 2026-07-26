#pragma once

#include <memory>
#include"../node.h"

namespace AST
{
	namespace Nodes
	{
		enum class UnaryOperator
		{
			Positive,
			Negative,
			Not,
		};

		class UnaryNode : public Node
		{
		public:
			UnaryNode(UnaryOperator op, std::shared_ptr<Node> target, LocationSpan loc)
                : m_operator(op), m_target(target), Node(loc) {}
			~UnaryNode() {}

        public:
            inline UnaryOperator GetOperator() const { return m_operator; }
            inline std::shared_ptr<Node> GetTarget() const { return m_target; }

		public:
			inline NodeType GetType() const override { return NodeType::Unary; }

		private:
			UnaryOperator m_operator;
			std::shared_ptr<Node> m_target;
		};
	}
}

