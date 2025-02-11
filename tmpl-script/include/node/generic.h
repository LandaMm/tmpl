
#include "../node.h"
#include "identifier.h"
#include <memory>

namespace AST
{
    namespace Nodes
    {
        class GenericNode : public Node
        {
        private:
            std::shared_ptr<IdentifierNode> m_target;
            std::shared_ptr<Node> m_type; // Another generic, e.g. list<list<int>> or just id list<int>
        
        public:
            GenericNode(std::shared_ptr<IdentifierNode> target, std::shared_ptr<Node> type, Location loc)
                : m_target(target), m_type(type), Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::Generic; }
        public:
            std::string Format() const override;
        public:
            inline std::shared_ptr<IdentifierNode> GetTarget() const { return m_target; }
            inline std::shared_ptr<Node> GetTypeNode() const { return m_type; }
        };
    }
}

