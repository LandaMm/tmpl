
#include"../node.h"

namespace Compiler
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
		private:
			std::shared_ptr<Node> m_left;
			std::shared_ptr<Node> m_right;
			ConditionType m_operator;
		public:
			Condition() : m_left(nullptr), m_right(nullptr), m_operator(ConditionType::None) { }
			~Condition() { }
		public:
			inline NodeType GetType() const override { return NodeType::Condition; }
		private:
			// TODO:
			std::string Format() const override { return "Condition"; }
		public:
			void SetLeft(std::shared_ptr<Node> left) { m_left = left; }
			void SetRight(std::shared_ptr<Node> right) { m_right = right; }
			void SetOp(ConditionType op) { m_operator = op; }
		};

		class TernaryNode : public Node
		{
		private:
			std::shared_ptr<Node> m_condition;
			std::shared_ptr<Node> m_left;
			std::shared_ptr<Node> m_right;
		public:
			inline NodeType GetType() const override { return NodeType::Ternary; }
		private:
			// TODO:
			std::string Format() const override { return "Ternary"; }
		public:
			void SetLeft(std::shared_ptr<Node> left) { m_left = left; }
			void SetRight(std::shared_ptr<Node> right) { m_right = right; }
			void SetCondition(std::shared_ptr<Node> condition) { m_condition = condition; }
		};
	}
}
