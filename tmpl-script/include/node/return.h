

#ifndef RETURN_H
#define RETURN_H
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
            std::string Format() const override { return "Return"; }
        public:
            inline std::shared_ptr<Node> GetValue() const { return m_value; }
        };
    }
}

#endif // RETURN_H

