#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H
#include <vector>
#include <memory>
#include "../node.h"
#include "../location.h"
#include "include/node/type.h"
#include "statement.h"
#include "identifier.h"

namespace AST
{
	namespace Nodes
	{
		class FunctionCall : public Node
		{
		private:
			std::shared_ptr<Node> m_callee;
			std::vector<std::shared_ptr<Node>> m_args;
            std::vector<std::shared_ptr<TypeNode>> m_generics;

		public:
			std::string Format() const override;
			inline NodeType GetType() const override { return NodeType::FunctionCall; }

		public:
			FunctionCall(std::shared_ptr<Node> callee, Location loc)
                : m_callee(callee),
                  m_args(std::vector<std::shared_ptr<Node>>()),
                  m_generics(std::vector<std::shared_ptr<TypeNode>>()),
                  Node(loc) {}
			~FunctionCall() {}

        public:
            void AddGeneric(std::shared_ptr<TypeNode> generic) { m_generics.push_back(generic); }
            std::shared_ptr<TypeNode> GetGeneric(unsigned int index) const { return m_generics[index]; }
            unsigned int GetGenericsSize() const { return m_generics.size(); }

		public:
			inline std::shared_ptr<Node> GetCallee() const { return m_callee; }

        public:
            void AddArgument(std::shared_ptr<Node> arg) { m_args.push_back(arg); }
            unsigned int GetArgumentsSize() const { return m_args.size(); }
            std::shared_ptr<Node> GetArgument(unsigned int index) const { return m_args[index]; }
		};

        class FunctionParam
        {
        private:
            std::shared_ptr<TypeNode> m_type;
            std::shared_ptr<IdentifierNode> m_name;
        public:
            FunctionParam(std::shared_ptr<TypeNode> type, std::shared_ptr<IdentifierNode> name)
                : m_type(type), m_name(name) { }
            ~FunctionParam() = default;
        public:
            inline std::shared_ptr<TypeNode> GetType() const { return m_type; }
            inline std::shared_ptr<IdentifierNode> GetName() const { return m_name; }
        };

        enum class FunctionModifier
        {
            Construct,
            Cast,
            None,
        };

        class FunctionDeclaration : public Node
        {
        private:
            std::shared_ptr<Node> m_name;
            std::vector<std::shared_ptr<FunctionParam>> m_params;
            std::shared_ptr<TypeNode> m_ret_type;
            std::shared_ptr<Statements::StatementsBody> m_body;
            std::vector<std::shared_ptr<TemplateGeneric>> m_generics;
            FunctionModifier m_modifier;
        private:
            size_t m_index;
        public:
            FunctionDeclaration(
                    std::shared_ptr<Node> name,
                    std::shared_ptr<Statements::StatementsBody> body,
                    FunctionModifier modifier,
                    Location loc
                    )
                : m_name(name),
                m_params(std::vector<std::shared_ptr<FunctionParam>>()),
                m_generics(std::vector<std::shared_ptr<TemplateGeneric>>()),
                m_ret_type(nullptr),
                m_body(body),
                m_modifier(modifier),
                m_index(0),
                Node(loc) { }
            ~FunctionDeclaration() = default;
        public:
            void AddParam(std::shared_ptr<FunctionParam> param);
            void AddGeneric(std::shared_ptr<TemplateGeneric> generic) { m_generics.push_back(generic); }
            void SetReturnType(std::shared_ptr<TypeNode> retType) { m_ret_type = retType; }
        public:
            inline std::shared_ptr<Node> GetName() const { return m_name; }
            inline std::shared_ptr<TypeNode> GetReturnType() const { return m_ret_type; }
            inline std::shared_ptr<Statements::StatementsBody> GetBody() const { return m_body; }
            inline FunctionModifier GetModifier() const { return m_modifier; }
        public:
            inline std::shared_ptr<FunctionParam> GetParam(unsigned int index)
                { return m_params[index]; }
            inline unsigned int GetParamsSize() const { return m_params.size(); }
        public:
            inline std::shared_ptr<TemplateGeneric> GetGeneric(unsigned int index)
                { return m_generics[index]; }
            inline unsigned int GetGenericsSize() const { return m_generics.size(); }
		public:
			inline NodeType GetType() const override { return NodeType::FnDecl; }
            std::string Format() const override;
        };
	}
}

#endif
