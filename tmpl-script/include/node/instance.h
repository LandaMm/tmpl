#ifndef INSTANCE_NODE_H
#define INSTANCE_NODE_H

#include "../node.h"
#include "function.h"
#include "type.h"

namespace AST
{
    namespace Nodes
    {
        class InstanceNode : public Node
        {
        private:
            std::shared_ptr<TypeNode> m_target;
            std::shared_ptr<FunctionCall> m_fcall;

        public:
            InstanceNode(std::shared_ptr<TypeNode> target, std::shared_ptr<FunctionCall> fcall, Location loc)
                : m_target(target), m_fcall(fcall), Node(loc) { }
            ~InstanceNode() = default;
        public:
            inline NodeType GetType() const override { return NodeType::Instance; }
            std::string Format() const override;

        public:
            inline std::shared_ptr<TypeNode> GetTarget() const { return m_target; }
            inline std::shared_ptr<FunctionCall> GetFunctionCall() const { return m_fcall; }
        };
    }
}

#endif // INSTANCE_NODE_H

