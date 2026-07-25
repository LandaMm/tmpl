#pragma once

#include <memory>
#include <vector>

#include "../node.h"
#include "include/node/identifier.hpp"

namespace AST
{
    namespace Nodes
    {
        class TypeNode : public Node
        {
        public:
            using PId = std::shared_ptr<IdentifierNode>;

        public:
            TypeNode(PId target, Location loc)
                : m_typename(target), Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::Type; };

        public:
            void AddGenericType(std::shared_ptr<TypeNode> typ) { m_generics.push_back(typ); }
            inline unsigned int GetGenericsSize() const { return m_generics.size(); }
            std::shared_ptr<TypeNode> GetGeneric(unsigned int index) const { return m_generics[index]; }

        public:
            inline PId GetTypeName() const { return m_typename; }

        private:
            PId m_typename;
            std::vector<std::shared_ptr<TypeNode>> m_generics;
        };

        class TemplateGeneric
        {
        public:
            TemplateGeneric(std::string name, Location loc)
                : m_name(name), m_loc(loc) { }

        public:
            inline std::string GetName() const { return m_name; }
            inline Location GetLocation() const { return m_loc; }

        private:
            std::string m_name;
            Location m_loc;
            // TODO: base and default type
        };

        class TypeDfNode : public Node
        {
        public:
            using PId = std::shared_ptr<IdentifierNode>;
            using PType = std::shared_ptr<TypeNode>;
            using PTG = std::shared_ptr<TemplateGeneric>;

        public:
            TypeDfNode(PId name, Location loc)
                : m_name(name),
                  m_value(nullptr),
                  m_generics(std::vector<PTG>()),
                  Node(loc) { }

        public:
            inline NodeType GetType() const override { return NodeType::TypeDf; };

        public:
            void SetValue(PType value) { m_value = value; }

        public:
            void AddGeneric(PTG generic) { m_generics.push_back(generic); } 
            unsigned int GetGenericsSize() const { return m_generics.size(); }
            PTG GetGeneric(unsigned int index) const { return m_generics[index]; }

            std::vector<PTG>* GetGenIterator() { return &m_generics; }

        public:
            inline PId GetTypeName() const { return m_name; }
            inline PType GetTypeValue() const { return m_value; }

        private:
            PId m_name;
            PType m_value;

            std::vector<PTG> m_generics;
        };
    }
}

