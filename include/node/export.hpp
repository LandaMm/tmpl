#pragma once

#include "node.h"
#include <memory>

namespace AST
{
    namespace Nodes
    {
        class ExportStatement : public Node
        {
        public:
            ExportStatement(std::shared_ptr<Node> target, Location loc)
                : m_target(target), Node(loc) { }
            ~ExportStatement() = default;
        public:
            inline NodeType GetType() const override { return NodeType::Export; }
        public:
            inline std::shared_ptr<Node> GetTarget() const { return m_target; }
        private:
            std::shared_ptr<Node> m_target;
        };
    }
}

