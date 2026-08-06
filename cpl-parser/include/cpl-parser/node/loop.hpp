#pragma once

#include "cpl-parser/node.h"
#include "cpl-parser/node/statement.hpp"

namespace AST::Nodes
{
    class BreakNode : public Node
    {
    public:
        BreakNode(LocationSpan loc) : Node(loc) { }
	public:
		inline NodeType GetType() const override { return NodeType::Break; };
    };

    class WhileNode : public Node
    {
    private:
        using PNode = Node*;
        using PBody = Statements::StatementsBody*;

    public:
        WhileNode(PNode condition, PBody body, LocationSpan loc)
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
        using PNode = Node*;
        using PBody = Statements::StatementsBody*;
    public:
        ForLoopNode(PNode decl, PNode condition, PNode assignment, PBody body, LocationSpan loc)
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

