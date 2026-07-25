#pragma once

#include "../node.h"
#include "type.hpp"

namespace AST
{
    namespace Nodes
    {
        class CastNode : public Node
        {
        private:
            using PTypeNode = std::shared_ptr<TypeNode>;
            using PNode = std::shared_ptr<Node>;
        public:
            CastNode(PTypeNode typ, PNode expr, Location loc)
                : m_type(typ), m_expr(expr), Node(loc) { }
        public:
            inline NodeType GetType() const override { return NodeType::Cast; };
        public:
            inline PTypeNode GetTypeNode() const { return m_type; }
            inline PNode GetExpr() const { return m_expr; }
        private:
            std::shared_ptr<TypeNode> m_type;
            std::shared_ptr<Node> m_expr;
        };
    }
}

