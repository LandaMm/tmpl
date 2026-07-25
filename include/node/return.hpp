#pragma once

#include <memory>
#include "../node.h"

namespace AST
{
    namespace Nodes
    {
        class ReturnNode : public Node
        {
        private:
            std::shared_ptr<Node> m_value;
        public:
            ReturnNode(std::shared_ptr<Node> value, Location loc)
                : m_value(value), Node(loc) { }
            ~ReturnNode() = default;
        public:
            inline NodeType GetType() const override { return NodeType::Return; }
        public:
            inline std::shared_ptr<Node> GetValue() const { return m_value; }
        };
    }
}

