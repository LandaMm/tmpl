#pragma once

#include<string>
#include<memory>

#include"../node.h"
#include "cpl-parser/node/symbol.hpp"
#include "cpl-parser/node/type.hpp"

namespace AST
{
	namespace Nodes
	{
		class VariableDeclaration : public Symbol
		{
		public:
			VariableDeclaration(Type* type, String* name, Node* value, SymbolFlag symbolFlag, LocationSpan loc)
				: m_type(type), m_name(name), m_value(value), Symbol(SymbolType::VARIABLE, symbolFlag, loc) { }

		public:
			inline NodeType GetType() const override { return NodeType::VarDecl; }

		public:
			inline Type* GetValueType() { return m_type; }
			inline Node* GetValue() { return m_value; }
			inline String* GetName() { return m_name; }
			inline bool HasValue() { return m_value != nullptr; }

		private:
			Type* m_type;
			String* m_name;
			Node* m_value; // nullable
		};

		class VarDeclaration : public Node
		{
		public:
			VarDeclaration(Type* type, std::string* name, Node* value, bool editable, LocationSpan loc)
				: m_type(type), m_name(name), m_value(value), m_editable(editable), Node(loc) { }
			VarDeclaration(Type* type, std::string* name, LocationSpan loc)
				: m_type(type), m_name(name), m_value(nullptr), m_editable(true), Node(loc) { }

		public:
			inline NodeType GetType() const override { return NodeType::VarDecl; }

		public:
			inline Type* GetType() { return m_type; }
			inline Node* GetValue() { return m_value; }
			inline std::string* GetName() { return m_name; }
			inline bool HasValue() { return m_value != nullptr; }
			inline bool Editable() { return m_editable; }

		private:
			Type* m_type;
			std::string* m_name;
			Node* m_value; // nullable
			bool m_editable;
		};
	}
}

