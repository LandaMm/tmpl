#pragma once

#include <memory>
#include "location.h"
#include "node.h"

namespace AST::Nodes
{
    enum class AssignOperator
    {
        Declare,
        Assign,
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
        AssignmentNode(PNode assignee, PNode value, AssignOperator op, LocationSpan loc)
            : m_assignee(assignee), m_value(value), m_operator(op), Node(loc) { }

	public:
		inline NodeType GetType() const override { return NodeType::Assign; }

    public:
        inline PNode GetAssignee() const { return m_assignee; }
        inline PNode GetValue() const { return m_value; }
        inline AssignOperator GetAssignOp() const { return m_operator; }
    };
}

