#ifndef PARSER_H
#define PARSER_H
#include"lexer.h"
#include"node.h"
#include"node/program.h"
#include"node/identifier.h"
#include"node/function.h"
#include"node/list.h"
#include"error.h"
#include<memory>

namespace AST
{
	class Parser
	{
	private:
		std::shared_ptr<Nodes::ProgramNode> m_root;
		std::shared_ptr<Lexer> m_lexer;
	public:
		Parser(std::shared_ptr<Lexer> lexer) : m_lexer(lexer), m_root(std::make_shared<Nodes::ProgramNode>()) {}
		~Parser() { }
	public:
		void Parse();
	public:
		inline std::shared_ptr<Node> GetRoot() const { return m_root; }
	private:
		Prelude::ErrorManager& GetErrorManager();
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
	private: // Object Member
		std::shared_ptr<Node> ObjectMember(std::shared_ptr<Node> obj);
	private: // Statements
		std::shared_ptr<Node> Statement();
		std::shared_ptr<Node> IfElseStatement();
		std::shared_ptr<Node> VariableDeclaration();
	};
}

#endif
