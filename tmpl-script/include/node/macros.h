#ifndef MACROS_H
#define MACROS_H

#include <memory>
#include "../node.h"
#include "include/node/function.h"

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

        class ExternMacro : public Node
        {
        private:
            std::shared_ptr<Node> m_target;
        public:
            ExternMacro(std::shared_ptr<Node> target, Location loc)
                : m_target(target), Node(loc) { }
            ~ExternMacro() = default;
        public:
            inline std::shared_ptr<FunctionDeclaration> GetFnSignature() const { return std::dynamic_pointer_cast<FunctionDeclaration>(m_target); }
        public:
            inline NodeType GetType() const override { return NodeType::Extern; };

        public:
            std::string Format() const override;
        };
    }
}

#endif // MACROS_H
