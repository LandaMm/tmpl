#pragma once

#include "cpl-parser/node.h"
#include "cpl-parser/node/type.hpp"

namespace AST
{
    namespace Nodes
    {
        class CastNode : public Node
        {
        private:
            using PTypeNode = Type*;
            using PNode = Node*;
        public:
            CastNode(PTypeNode typ, PNode expr, LocationSpan loc)
                : m_type(typ), m_expr(expr), Node(loc) { }
        public:
            inline NodeType GetType() const override { return NodeType::Cast; };
        public:
            inline PTypeNode GetTypeNode() const { return m_type; }
            inline PNode GetExpr() const { return m_expr; }
        private:
            Type* m_type;
            Node* m_expr;
        };
    }
}

