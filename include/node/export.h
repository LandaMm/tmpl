
#include "../node.h"
#include <memory>

namespace AST
{
    namespace Nodes
    {
        class ExportStatement : public Node
        {
        private:
            std::shared_ptr<Node> m_target;
        public:
            ExportStatement(std::shared_ptr<Node> target, Location loc)
                : m_target(target), Node(loc) { }
            ~ExportStatement() = default;
        public:
            inline NodeType GetType() const override { return NodeType::Export; }

        public:
            std::string Format() const override;

        public:
            inline std::shared_ptr<Node> GetTarget() const { return m_target; }
        };
    }
}

