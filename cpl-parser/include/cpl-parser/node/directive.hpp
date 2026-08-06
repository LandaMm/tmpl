#pragma once

#include <memory>
#include "cpl-parser/node.h"
#include "cpl-parser/node/function.hpp"

#include "cpl-basics/string.hpp"

namespace AST
{
    namespace Nodes
    {
        class ImportDirective : public Node
        {
        private:
            String m_module;
        public:
            ImportDirective(String module, LocationSpan loc)
                : m_module(module), Node(loc) { }
            ~ImportDirective() = default;
        public:
            inline String GetModule() const { return m_module; }
        public:
            inline NodeType GetType() const override { return NodeType::Require; };
        };

        class ExternDirective : public Node
        {
        private:
            Node* m_target;
        public:
            ExternDirective(Node* target, LocationSpan loc)
                : m_target(target), Node(loc) { }
            ~ExternDirective() = default;
        public:
            inline FunctionDeclaration* GetFnSignature() const { return reinterpret_cast<FunctionDeclaration*>(m_target); }
        public:
            inline NodeType GetType() const override { return NodeType::Extern; };
        };
    }
}

