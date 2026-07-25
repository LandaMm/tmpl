#ifndef LITERAL_H
#define LITERAL_H
#include <memory>
#include "../node.h"

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

		class LiteralNode : public Node
		{
		public:
			class ValueHolder
			{
			public:
				virtual ~ValueHolder() = default;
			};

			template <typename T>
			class TypedValueHolder : public ValueHolder
			{
			private:
				std::shared_ptr<T> m_value;

			public:
				TypedValueHolder(std::shared_ptr<T> value) : m_value(value) {}

			public:
				inline std::shared_ptr<T> GetValue() const { return m_value; }
			};

		private:
			LiteralType m_type;
			std::shared_ptr<ValueHolder> m_value;

		public:
			LiteralNode(LiteralType type, std::shared_ptr<ValueHolder> value, Location loc)
                : m_type(type), m_value(value), Node(loc) {}
			~LiteralNode() {}

		public:
			inline NodeType GetType() const override { return NodeType::Literal; }

		public:
			std::string Format() const override;

		public:
			inline LiteralType GetLiteralType() const { return m_type; }
			template <typename T>
			std::shared_ptr<T> GetValue() const
			{
				if (!m_value)
					return nullptr;
				std::shared_ptr<TypedValueHolder<T>> holder = std::dynamic_pointer_cast<TypedValueHolder<T>>(m_value);
				return holder ? holder->GetValue() : nullptr;
			}
		};
	}
}

#endif
