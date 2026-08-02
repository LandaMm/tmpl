#pragma once

#include <memory>
#include "node.h"

namespace AST
{
    namespace Nodes
    {
        class ReturnNode : public Node
        {
        private:
            Node* m_value;
        public:
            ReturnNode(Node* value, LocationSpan loc)
                : m_value(value), Node(loc) { }
            ~ReturnNode() = default;
        public:
            inline NodeType GetType() const override { return NodeType::Return; }
        public:
            inline Node* GetValue() const { return m_value; }
        };
    }
}

