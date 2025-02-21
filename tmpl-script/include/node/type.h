
#ifndef TYPE_NODE_H
#define TYPE_NODE_H

#include <memory>
#include "../node.h"
#include "include/node/identifier.h"

namespace AST
{
    namespace Nodes
    {
        class TypeNode : public Node
        {
        public:
            using PId = std::shared_ptr<IdentifierNode>;
        private:
            PId m_typename;
            // TODO:
            // std::vector<std::shared_ptr<...>> m_generics;

        public:
            TypeNode(PId target, Location loc)
                : m_typename(target), Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::Type; };
            std::string Format() const override;

        public:
            inline PId GetTypeName() const { return m_typename; }
        };

        class GenericNode : public Node
        {
        public:
            using PId = std::shared_ptr<IdentifierNode>;
        private:
            PId m_name;
            // TODO:
            // std::shared_ptr<TypeNode> m_basetype;
            // std::shared_ptr<TypeNode> m_default;

        public:
            GenericNode(PId name, Location loc)
                : m_name(name), Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::Generic; };
            std::string Format() const override;

        public:
            PId GetName() const { return m_name; }
        };

        class TypeTemplateNode : public Node
        {
        public:
            using PId = std::shared_ptr<IdentifierNode>;
        private:
            PId m_typename;
            // TODO:
            // std::vector<std::shared_ptr<...>> m_generics;

        public:
            TypeTemplateNode(PId target, Location loc)
                : m_typename(target), Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::TypeTemplate; };
            std::string Format() const override;

        public:
            inline PId GetTypeName() const { return m_typename; }
        };

        class TypeDfNode : public Node
        {
        public:
            using PTypeTmpl = std::shared_ptr<TypeTemplateNode>;
            using PType = std::shared_ptr<TypeNode>;
        private:
            PTypeTmpl m_template;
            PType m_value;

        public:
            TypeDfNode(PTypeTmpl tmpl, PType value, Location loc)
                : m_template(tmpl), m_value(value), Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::TypeDf; };
            std::string Format() const override;

        public:
            inline PTypeTmpl GetTypeTemplate() const { return m_template; }
            inline PType GetTypeValue() const { return m_value; }
        };
    }
}

#endif // TYPE_NODE_H

