
#ifndef LOOP_NODE_H_
#define LOOP_NODE_H_

#include "../node.h"
#include "include/node/statement.h"

namespace AST::Nodes
{
    class WhileNode : public Node
    {
    private:
        using PNode = std::shared_ptr<Node>;
        using PBody = std::shared_ptr<Statements::StatementsBody>;
    private:
        PNode m_condition;
        PBody m_body;

    public:
        WhileNode(PNode condition, PBody body, Location loc)
            : m_condition(condition), m_body(body), Node(loc) { }

	public:
		inline NodeType GetType() const override { return NodeType::While; };
		std::string Format() const override;
        inline bool IsBlock() override { return true; }

    public:
        inline PNode GetCondition() const { return m_condition; }
        inline PBody GetBody() const { return m_body; }
    };
}

#endif // LOOP_NODE_H_

