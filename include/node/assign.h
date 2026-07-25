
#ifndef ASSIGN_NODE_H_
#define ASSIGN_NODE_H_

#include <memory>
#include "include/location.h"
#include "include/node.h"

namespace AST::Nodes
{
    enum class AssignOperator
    {
        Reassign,
        Add,
        Subtract,
        Multiply,
        Divide
    };

    class AssignmentNode : public Node
    {
    public:
        using PNode = std::shared_ptr<Node>;
    private:
        AssignOperator m_operator;
        PNode m_assignee;
        PNode m_value;

    public:
        AssignmentNode(PNode assignee, PNode value, AssignOperator op, Location loc)
            : m_assignee(assignee), m_value(value), m_operator(op), Node(loc) { }

	public:
		inline NodeType GetType() const override { return NodeType::Assign; }

	public:
		std::string Format() const override;

    public:
        inline PNode GetAssignee() const { return m_assignee; }
        inline PNode GetValue() const { return m_value; }
        inline AssignOperator GetAssignOp() const { return m_operator; }
    };
}

#endif // ASSIGN_NODE_H_

