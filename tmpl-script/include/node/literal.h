#ifndef LITERAL_H
#define LITERAL_H
#include"../node.h"


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

		class LiteralNode : public Node {
		private:
			LiteralType m_type;
			void* m_value;
		public:
			LiteralNode(LiteralType type, void* value) : m_type(type), m_value(value) { }
			~LiteralNode() { }
		public:
			inline NodeType GetType() const override { return NodeType::Literal; }
		private:
			std::string Format() const override;
		public:
			inline LiteralType GetLiteralType() const { return m_type; }
			template<typename T>
			inline T* GetValue() const { return (T*)m_value; }
		};
	}
}

#endif