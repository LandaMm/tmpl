#ifndef CAST_NODE_H
#define CAST_NODE_H

#include "../node.h"
#include "type.h"

namespace AST
{
    namespace Nodes
    {
        class CastNode : public Node
        {
        private:
            using PTypeNode = std::shared_ptr<TypeNode>;
            using PNode = std::shared_ptr<Node>;
        private:
            std::shared_ptr<TypeNode> m_type;
            std::shared_ptr<Node> m_expr;

        public:
            CastNode(PTypeNode typ, PNode expr, Location loc)
                : m_type(typ), m_expr(expr), Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::Cast; };

        public:
            std::string Format() const override;
        };
    }
}

#endif // CAST_NODE_H

