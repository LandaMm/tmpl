#ifndef PARSER_H
#define PARSER_H
#include "include/node/cast.h"
#include "lexer.h"
#include "node.h"
#include "node/program.h"
#include "node/identifier.h"
#include "node/function.h"
#include "node/list.h"
#include "node/type.h"
#include "error.h"
#include <memory>

namespace AST
{
	class Parser
	{
	private:
		std::shared_ptr<Nodes::ProgramNode> m_root;
		std::shared_ptr<Lexer> m_lexer;

    private:
        std::vector<AST::Location> m_breaks;

	public:
		Parser(std::shared_ptr<Lexer> lexer)
            : m_lexer(lexer), m_breaks(std::vector<AST::Location>()),
              m_root(std::make_shared<Nodes::ProgramNode>()) {}
		~Parser() {}

	public:
		void Parse();

	public:
		inline std::shared_ptr<Node> GetRoot() const { return m_root; }
        inline std::string GetFilename() const { return m_lexer->GetFilename(); }

	private:
		Prelude::ErrorManager &GetErrorManager();
		void Eat(TokenType type);
		TokenType Peek();

	private: // Common
		std::shared_ptr<Nodes::IdentifierNode> Id();
		std::shared_ptr<Nodes::FunctionCall> FunctionCall(std::shared_ptr<Node> callee);
		std::shared_ptr<Nodes::ListNode> List();

	private: // Expression
		std::shared_ptr<Node> Factor();
		std::shared_ptr<Node> Term();
		std::shared_ptr<Node> Expr();
		std::shared_ptr<Node> Cond();
		std::shared_ptr<Node> Ternary();
		std::shared_ptr<Node> Assignment();

    private: // Checkers
        bool IsTypeCastAhead();

	private: // Object Member
		std::shared_ptr<Node> ObjectMember(std::shared_ptr<Node> obj);

    private: // Types
        std::shared_ptr<Nodes::TypeNode> Type();
        std::shared_ptr<Nodes::TypeNode> Type(std::shared_ptr<Nodes::IdentifierNode> target);
        std::shared_ptr<Nodes::TemplateGeneric> TmplGeneric();
        std::shared_ptr<Nodes::TypeDfNode> TypeDfStatement();
        std::shared_ptr<Nodes::CastNode> Cast(std::shared_ptr<Nodes::TypeNode> typ);

	private: // Statements
		std::shared_ptr<Node> Statement();
		std::shared_ptr<Node> IfElseStatement();
		std::shared_ptr<Node> VariableDeclaration();
		std::shared_ptr<Node> ProcedureDeclaration();
        std::shared_ptr<Node> ReturnStatement();
        std::shared_ptr<Nodes::FunctionDeclaration> FunctionSignature();
        std::shared_ptr<Node> FunctionDeclaration();
        std::shared_ptr<Node> ExportStmt();
        std::shared_ptr<Node> WhileLoop();
        std::shared_ptr<Node> ForLoop();
        std::shared_ptr<Node> BreakStmt();
    private: // Macros
        std::shared_ptr<Node> RequireStatement();
        std::shared_ptr<Node> ExternStatement();
	};
}

#endif
