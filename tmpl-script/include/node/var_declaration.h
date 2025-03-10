

#ifndef VAR_DECLARATION_H
#define VAR_DECLARATION_H
#include<string>
#include<memory>
#include"../node.h"
#include "include/node/type.h"

namespace AST
{
	namespace Nodes
	{
		class VarDeclaration : public Node
		{
		private:
			std::shared_ptr<TypeNode> m_type;
			std::shared_ptr<std::string> m_name;
			std::shared_ptr<Node> m_value; // nullable
			bool m_editable;
		public:
			inline NodeType GetType() const override { return NodeType::VarDecl; }
		public:
			std::string Format() const override { return "VarDecl()"; }
		public:
			VarDeclaration(std::shared_ptr<TypeNode> type, std::shared_ptr<std::string> name, std::shared_ptr<Node> value, bool editable, Location loc)
				: m_type(type), m_name(name), m_value(value), m_editable(editable), Node(loc) { }
			VarDeclaration(std::shared_ptr<TypeNode> type, std::shared_ptr<std::string> name, Location loc)
				: m_type(type), m_name(name), m_value(nullptr), m_editable(true), Node(loc) { }
		public:
			inline std::shared_ptr<TypeNode> GetType() { return m_type; }
			inline std::shared_ptr<Node> GetValue() { return m_value; }
			inline std::shared_ptr<std::string> GetName() { return m_name; }
			inline bool HasValue() { return m_value != nullptr; }
			inline bool Editable() { return m_editable; }
		};
	}
}

#endif
