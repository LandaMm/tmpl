#pragma once

#include <memory>
#include <vector>

#include "node.h"
#include "node/symbol.hpp"
#include "node/identifier.hpp"

namespace AST
{
    namespace Nodes
    {
        class TypeNode : public Node
        {
        public:
            using PId = IdentifierNode*;

        public:
            TypeNode(PId target, LocationSpan loc)
                : m_typename(target), Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::Type; };

        public:
            inline PId GetTypeName() const { return m_typename; }

        private:
            PId m_typename;
        };

        class TypeDeclaration : public Node
        {
        public:
            using PId = IdentifierNode*;
            using PType = TypeNode*;

        public:
            TypeDeclaration(PId name, LocationSpan loc)
                : m_name(name),
                  m_value(nullptr),
                  Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::TypeDf; };

        public:
            void SetValue(PType value) { m_value = value; }

        public:
            inline PId GetTypeName() const { return m_name; }
            inline PType GetTypeValue() const { return m_value; }

        private:
            PId m_name;
            PType m_value;
        };
    }
}

