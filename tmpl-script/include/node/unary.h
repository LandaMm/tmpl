#ifndef UNARY_H
#define UNARY_H
#include"../node.h"

namespace AST
{
	namespace Nodes
	{
		class UnaryNode : public Node
		{
		public:
			enum class UnaryOperator
			{
				Positive,
				Negative,
				Not,
			};
		private:
			UnaryOperator m_operator;
			std::shared_ptr<Node> m_target;
		public:
			UnaryNode(UnaryOperator op, std::shared_ptr<Node> target, Location loc)
                : m_operator(op), m_target(target), Node(loc) {}
			~UnaryNode() {}
		public:
			inline NodeType GetType() const override { return NodeType::Unary; }
		public:
			std::string Format() const override { return "UnaryNode"; }
		};
	}
}

#endif
