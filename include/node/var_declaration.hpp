#pragma once

#include<string>
#include<memory>

#include"../node.h"
#include "node/type.hpp"

namespace AST
{
	namespace Nodes
	{
		class VarDeclaration : public Node
		{
		public:
			VarDeclaration(TypeNode* type, std::string* name, Node* value, bool editable, LocationSpan loc)
				: m_type(type), m_name(name), m_value(value), m_editable(editable), Node(loc) { }
			VarDeclaration(TypeNode* type, std::string* name, LocationSpan loc)
				: m_type(type), m_name(name), m_value(nullptr), m_editable(true), Node(loc) { }

		public:
			inline NodeType GetType() const override { return NodeType::VarDecl; }

		public:
			inline TypeNode* GetType() { return m_type; }
			inline Node* GetValue() { return m_value; }
			inline std::string* GetName() { return m_name; }
			inline bool HasValue() { return m_value != nullptr; }
			inline bool Editable() { return m_editable; }

		private:
			TypeNode* m_type;
			std::string* m_name;
			Node* m_value; // nullable
			bool m_editable;
		};
	}
}

