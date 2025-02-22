
#ifndef ASSIGN_NODE_H_
#define ASSIGN_NODE_H_

#include <memory>
#include "include/location.h"
#include "include/node.h"

namespace AST::Nodes
{
    class AssignmentNode : public Node
    {
    public:
        using PNode = std::shared_ptr<Node>;
    private:
        PNode m_assignee;
        PNode m_value;

    public:
        AssignmentNode(PNode assignee, PNode value, Location loc)
            : m_assignee(assignee), m_value(value), Node(loc) { }

	public:
		inline NodeType GetType() const override { return NodeType::Assign; }

	public:
		std::string Format() const override;

    public:
        inline PNode GetAssignee() const { return m_assignee; }
        inline PNode GetValue() const { return m_value; }
    };
}

#endif // ASSIGN_NODE_H_

