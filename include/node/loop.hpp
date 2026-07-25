#pragma once

#include "../node.h"
#include "include/node/statement.hpp"

namespace AST::Nodes
{
    class BreakNode : public Node
    {
    public:
        BreakNode(Location loc) : Node(loc) { }
	public:
		inline NodeType GetType() const override { return NodeType::Break; };
    };

    class WhileNode : public Node
    {
    private:
        using PNode = std::shared_ptr<Node>;
        using PBody = std::shared_ptr<Statements::StatementsBody>;

    public:
        WhileNode(PNode condition, PBody body, Location loc)
            : m_condition(condition), m_body(body), Node(loc) { }

	public:
		inline NodeType GetType() const override { return NodeType::While; };
        inline bool IsBlock() override { return true; }

    public:
        inline PNode GetCondition() const { return m_condition; }
        inline PBody GetBody() const { return m_body; }
    private:
        PNode m_condition;
        PBody m_body;
    };

    class ForLoopNode : public Node
    {
    private:
        using PNode = std::shared_ptr<Node>;
        using PBody = std::shared_ptr<Statements::StatementsBody>;
    public:
        ForLoopNode(PNode decl, PNode condition, PNode assignment, PBody body, Location loc)
            : m_decl(decl),
              m_condition(condition),
              m_assignment(assignment),
              m_body(body),
              Node(loc) { }

	public:
		inline NodeType GetType() const override { return NodeType::For; };
        inline bool IsBlock() override { return true; }

    public:
        inline PNode GetDecl() const { return m_decl; }
        inline PNode GetCondition() const { return m_condition; }
        inline PNode GetAssignment() const { return m_assignment; }
        inline PBody GetBody() const { return m_body; }
    private:
        PNode m_decl;
        PNode m_condition;
        PNode m_assignment;
        PBody m_body;
    };
}

