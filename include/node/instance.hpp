#pragma once

#include "node.h"

#include "function.hpp"

#include "node/identifier.hpp"

namespace AST
{
    namespace Nodes
    {
        class InstanceNode : public Node
        {
        public:
            InstanceNode(std::shared_ptr<IdentifierNode> target, std::shared_ptr<FunctionCall> fcall, Location loc)
                : m_target(target), m_fcall(fcall), Node(loc) { }
            ~InstanceNode() = default;
        public:
            inline NodeType GetType() const override { return NodeType::Instance; }

        public:
            inline std::shared_ptr<IdentifierNode> GetTarget() const { return m_target; }
            inline std::shared_ptr<FunctionCall> GetFunctionCall() const { return m_fcall; }
        private:
            std::shared_ptr<IdentifierNode> m_target;
            std::shared_ptr<FunctionCall> m_fcall;
        };
    }
}

