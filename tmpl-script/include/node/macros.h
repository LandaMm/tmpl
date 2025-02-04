

#include "../node.h"

namespace AST
{
    namespace Nodes
    {
        class RequireMacro : public Node
        {
        private:
            std::string m_module;
        public:
            RequireMacro(std::string module, Location loc)
                : m_module(module), Node(loc) { }
            ~RequireMacro() = default;
        public:
            inline std::string GetModule() const { return m_module; }
        public:
            inline NodeType GetType() const override { return NodeType::Require; };

        public:
            std::string Format() const override;
        };
    }
}

