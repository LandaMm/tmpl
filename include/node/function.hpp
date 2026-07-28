#pragma once

#include <vector>
#include <memory>

#include "node.h"
#include "location.h"
#include "node/type.hpp"
#include "statement.hpp"
#include "identifier.hpp"

namespace AST
{
	namespace Nodes
	{
		class FunctionCall : public Node
		{
		public:
			FunctionCall(Node* callee, LocationSpan loc)
                : m_callee(callee),
                  m_args(std::vector<Node*>()),
                  m_generics(std::vector<TypeNode*>()),
                  Node(loc) {}
			~FunctionCall() {}
		public:
			inline NodeType GetType() const override { return NodeType::FunctionCall; }
        public:
            void AddGeneric(TypeNode* generic) { m_generics.push_back(generic); }
            TypeNode* GetGeneric(unsigned int index) const { return m_generics[index]; }
            unsigned int GetGenericsSize() const { return m_generics.size(); }
		public:
			inline Node* GetCallee() const { return m_callee; }
        public:
            void AddArgument(Node* arg) { m_args.push_back(arg); }
            unsigned int GetArgumentsSize() const { return m_args.size(); }
            Node* GetArgument(unsigned int index) const { return m_args[index]; }
		private:
			Node* m_callee;
			std::vector<Node*> m_args;
            std::vector<TypeNode*> m_generics;
		};

        class FunctionParam
        {
        public:
            FunctionParam(TypeNode* type, IdentifierNode* name)
                : m_type(type), m_name(name) { }
            ~FunctionParam() = default;

        public:
            inline TypeNode* GetType() const { return m_type; }
            inline IdentifierNode* GetName() const { return m_name; }

        private:
            TypeNode* m_type;
            IdentifierNode* m_name;
        };

        class FunctionDeclaration : public Node
        {
        private:
            size_t m_index;
        public:
            FunctionDeclaration(
                    Node* name,
                    Statements::StatementsBody* body,
                    LocationSpan loc
                    )
                : m_name(name),
                m_params(std::vector<FunctionParam*>()),
                m_generics(std::vector<TemplateGeneric*>()),
                m_ret_type(nullptr),
                m_body(body),
                m_index(0),
                Node(loc) { }
            ~FunctionDeclaration() = default;
        public:
			void AddParam(FunctionParam* param) { m_params.push_back(param); }
            void AddGeneric(TemplateGeneric* generic) { m_generics.push_back(generic); }
            void SetReturnType(TypeNode* retType) { m_ret_type = retType; }
        public:
            inline Node* GetName() const { return m_name; }
            inline TypeNode* GetReturnType() const { return m_ret_type; }
            inline Statements::StatementsBody* GetBody() const { return m_body; }
        public:
            inline FunctionParam* GetParam(unsigned int index)
                { return m_params[index]; }
            inline unsigned int GetParamsSize() const { return m_params.size(); }
        public:
            inline TemplateGeneric* GetGeneric(unsigned int index)
                { return m_generics[index]; }
            inline unsigned int GetGenericsSize() const { return m_generics.size(); }
		public:
			inline NodeType GetType() const override { return NodeType::FnDecl; }
        private:
            Node* m_name;
            std::vector<FunctionParam*> m_params;
            TypeNode* m_ret_type;
            Statements::StatementsBody* m_body;
            std::vector<TemplateGeneric*> m_generics;
        };
	}
}
