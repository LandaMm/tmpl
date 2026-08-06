#pragma once

#include <vector>
#include <memory>

#include "cpl-parser/node.h"
#include "cpl-parser/node/symbol.hpp"
#include "cpl-parser/node/type.hpp"
#include "cpl-parser/node/statement.hpp"
#include "cpl-parser/node/identifier.hpp"

#include "cpl-parser/location.h"

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
                  Node(loc) {}
			~FunctionCall() {}
		public:
			inline NodeType GetType() const override { return NodeType::FunctionCall; }
		public:
			inline Node* GetCallee() const { return m_callee; }
        public:
            void AddArgument(Node* arg) { m_args.push_back(arg); }
            unsigned int GetArgumentsSize() const { return m_args.size(); }
            Node* GetArgument(unsigned int index) const { return m_args[index]; }
		private:
			Node* m_callee;
			std::vector<Node*> m_args;
		};

        class FunctionParam
        {
        public:
            FunctionParam(Type* type, IdentifierNode* name)
                : m_type(type), m_name(name) { }
            ~FunctionParam() = default;

        public:
            inline Type* GetType() const { return m_type; }
            inline IdentifierNode* GetName() const { return m_name; }

        private:
            Type* m_type;
            IdentifierNode* m_name;
        };

        class FunctionDeclaration : public Symbol
        {
        public:
            FunctionDeclaration(
				Node* name,
				Statements::StatementsBody* body,
                SymbolFlag symbolFlag,
				LocationSpan loc
			) : m_name(name),
                m_ret_type(nullptr),
                m_body(body),
                Symbol(SymbolType::FUNCTION, symbolFlag, loc) { }

            ~FunctionDeclaration() = default;
        public:
			void AddParam(FunctionParam* param) { m_params.push_back(param); }
            void SetReturnType(Type* retType) { m_ret_type = retType; }
        public:
            inline Node* GetName() const { return m_name; }
            inline Type* GetReturnType() const { return m_ret_type; }
            inline Statements::StatementsBody* GetBody() const { return m_body; }
        public:
            inline FunctionParam* GetParam(unsigned int index)
                { return m_params[index]; }
            inline unsigned int GetParamsSize() const { return m_params.size(); }
		public:
			inline NodeType GetType() const override { return NodeType::FnDecl; }
        private:
            Node* m_name;
            std::vector<FunctionParam*> m_params;
            Type* m_ret_type;

            Statements::StatementsBody* m_body;
        };
	}
}
