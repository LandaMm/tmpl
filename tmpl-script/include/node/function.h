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

		public:
			std::string Format() const override;
			inline NodeType GetType() const override { return NodeType::FunctionCall; }

		public:
			FunctionCall(std::shared_ptr<Node> callee, std::vector<std::shared_ptr<Node>> args, Location loc)
                : m_callee(callee), m_args(args), Node(loc) {}
			~FunctionCall() {}

		public:
			inline std::shared_ptr<Node> GetCallee() const { return m_callee; }
			inline std::vector<std::shared_ptr<Node>> *GetArgs() { return &m_args; }
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
            None,
        };

        class FunctionDeclaration : public Node
        {
        private:
            std::shared_ptr<Node> m_name;
            std::vector<std::shared_ptr<FunctionParam>> m_params;
            std::shared_ptr<TypeNode> m_ret_type;
            std::shared_ptr<Statements::StatementsBody> m_body;
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
                m_ret_type(nullptr),
                m_body(body),
                m_modifier(modifier),
                m_index(0),
                Node(loc) { }
            ~FunctionDeclaration() = default;
        public:
            void AddParam(std::shared_ptr<FunctionParam> param);
            void SetReturnType(std::shared_ptr<TypeNode> retType) { m_ret_type = retType; }
        public:
            inline std::shared_ptr<Node> GetName() const { return m_name; }
            inline std::shared_ptr<TypeNode> GetReturnType() const { return m_ret_type; }
            inline std::shared_ptr<Statements::StatementsBody> GetBody() const { return m_body; }
            inline FunctionModifier GetModifier() const { return m_modifier; }
        public:
            inline std::shared_ptr<FunctionParam> GetItem(unsigned int index)
                { return m_params[index]; }
            inline unsigned int GetSize() const { return m_params.size(); }
		public:
			inline NodeType GetType() const override { return NodeType::FnDecl; }
            std::string Format() const override;
        };
	}
}

#endif
