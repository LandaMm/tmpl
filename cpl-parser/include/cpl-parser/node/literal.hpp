#pragma once

#include <memory>
#include "cpl-parser/node.h"

namespace AST
{
	namespace Nodes
	{
		enum class LiteralType
		{
			_NULL,
			INT,
			FLOAT,
			DOUBLE,
			STRING,
			BOOL,
		};

		class ValueHolder
		{
		public:
			virtual ~ValueHolder() = default;
		};

		template <typename T>
		class TypedValueHolder : public ValueHolder
		{
		private:
			T* m_value;

		public:
			TypedValueHolder(T* value) : m_value(value) {}

		public:
			inline T* GetValue() const { return m_value; }
		};

		class LiteralNode : public Node
		{
		public:
			LiteralNode(LiteralType type, ValueHolder* value, LocationSpan loc)
                : m_type(type), m_value(value), Node(loc) {}
			~LiteralNode() {}

		public:
			inline NodeType GetType() const override { return NodeType::Literal; }

		public:
			inline LiteralType GetLiteralType() const { return m_type; }
			template <typename T>
			T* GetValue() const
			{
				if (!m_value)
					return nullptr;
				TypedValueHolder<T>* holder = dynamic_cast<TypedValueHolder<T>*>(m_value);
				return holder ? holder->GetValue() : nullptr;
			}
		private:
			LiteralType m_type;
			ValueHolder* m_value;
		};
	}
}

