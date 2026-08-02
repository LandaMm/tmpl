#pragma once

#include <memory>
#include <map>

#include "cpl-basics/def.hpp"
#include "cpl-basics/string.hpp"
#include "cpl-basics/array.hpp"
#include "node.h"
#include "node/literal.hpp"
#include "node/symbol.hpp"
#include "node/identifier.hpp"

namespace AST
{
    namespace Nodes
    {
        enum class TypeKind
        {
            UNKNOWN = 0,
            BASIC,
            POINTER,
            STRUCT,
            ENUM,
            COUNT_TYPE_KINDS,
        };

        class Type : public Node
        {
        public:
            Type(TypeKind kind, LocationSpan loc) noexcept
                : m_kind(kind), Node(loc) { }
        public:
            inline NodeType GetType() const override { return NodeType::Type; };
        public:
            inline TypeKind GetKind() const noexcept { return m_kind; }
        private:
            TypeKind m_kind = TypeKind::UNKNOWN;
        };

        class BasicType : public Type
        {
        public:
            BasicType(const String& name, LocationSpan loc)
                : m_name(name), Type(TypeKind::BASIC, loc) { }
        public:
            inline const String& GetName() const noexcept { return m_name; }
        private:
            String m_name;
        };

        class PointerType : public Type
        {
        public:
            PointerType(Node* targetType, LocationSpan loc)
                : m_targetType(targetType), Type(TypeKind::POINTER, loc) { }
        public:
            inline const Node* GetTargetType() const noexcept { return m_targetType; }
        private:
            Node* m_targetType = nullptr;
        };

        class StructType : public Type
        {
        public:
            StructType(std::map<String, const Type*>&& field_desc, LocationSpan loc)
                : m_field_desc(field_desc), Type(TypeKind::STRUCT, loc) { }
        public:
            inline const std::map<String, const Type*>& GetFieldsDesc() const noexcept { return m_field_desc; }
        private:
            std::map<String, const Type*> m_field_desc;
        };

        struct EnumField
        {
            String name;
            Uint64 value;
        };

        class EnumType : public Type
        {
        public:
            EnumType(Array<EnumField> fields, LocationSpan loc)
                : m_fields(fields), Type(TypeKind::ENUM, loc) { }
            EnumType(LocationSpan loc)
                : Type(TypeKind::ENUM, loc) { }
        public:
            inline void AddField(const EnumField& field) { m_fields.Push(field); }
        public:
            inline const Array<EnumField>& GetFields() const noexcept { return m_fields; }
        private:
            Array<EnumField> m_fields;
        };

        class TypeDeclaration : public Node
        {
        public:
            using PId = IdentifierNode*;
            using PType = Type*;

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

