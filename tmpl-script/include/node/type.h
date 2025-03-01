
#ifndef TYPE_NODE_H
#define TYPE_NODE_H

#include <memory>
#include <vector>
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
            std::vector<std::shared_ptr<TypeNode>> m_generics;

        public:
            TypeNode(PId target, Location loc)
                : m_typename(target), Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::Type; };
            std::string Format() const override;

        public:
            void AddGenericType(std::shared_ptr<TypeNode> typ) { m_generics.push_back(typ); }
            inline unsigned int GetGenericsSize() const { return m_generics.size(); }
            std::shared_ptr<TypeNode> GetGeneric(unsigned int index) const { return m_generics[index]; }

        public:
            inline PId GetTypeName() const { return m_typename; }
        };

        class TemplateGeneric
        {
        private:
            std::string m_name;
            Location m_loc;
            // TODO: base and default type
        public:
            TemplateGeneric(std::string name, Location loc)
                : m_name(name), m_loc(loc) { }

        public:
            inline std::string GetName() const { return m_name; }
            inline Location GetLocation() const { return m_loc; }
        };

        class TypeTemplateNode : public Node
        {
        public:
            using PId = std::shared_ptr<IdentifierNode>;
        private:
            PId m_typename;
            std::vector<std::shared_ptr<TemplateGeneric>> m_generics;

        public:
            TypeTemplateNode(PId target, Location loc)
                : m_typename(target), m_generics(std::vector<std::shared_ptr<TemplateGeneric>>()), Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::TypeTemplate; };
            std::string Format() const override;

        public:
            void AddTemplateGeneric(std::shared_ptr<TemplateGeneric> generic) { m_generics.push_back(generic); }
            inline unsigned int GetGenericsSize() const { return m_generics.size(); }
            inline std::shared_ptr<TemplateGeneric> GetTemplateGeneric(unsigned int index)
                const { return m_generics[index]; }

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

