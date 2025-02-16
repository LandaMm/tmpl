
#ifndef TYPE_NODE_H
#define TYPE_NODE_H

#include <memory>
#include "../node.h"
#include "include/node/identifier.h"

namespace AST
{
    namespace Nodes
    {
        class TypeNode : public Node
        {
        public:
            using PId = std::shared_ptr<IdentifierNode>;
        private:
            std::shared_ptr<IdentifierNode> m_typename;
            // TODO:
            // std::vector<std::shared_ptr<...>> m_generics;

        public:
            TypeNode(PId target, Location loc)
                : m_typename(target), Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::Type; };
            std::string Format() const override;

        public:
            inline PId GetTypeName() const { return m_typename; }
        };
    }
}

#endif // TYPE_NODE_H

