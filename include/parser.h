#pragma once

#include "node/cast.hpp"
#include "lexer.h"
#include "node.h"
#include "node/program.hpp"
#include "node/identifier.hpp"
#include "node/function.hpp"
#include "node/type.hpp"
#include "error.h"
#include "basics/allocator/arena.hpp"
#include <memory>

namespace AST
{
	class Parser
	{
	public:
		Parser(Lexer* lexer)
            : m_lexer(lexer),
              m_root(nullptr)
		{
			m_root = m_arena.Alloc<Nodes::ProgramNode>();
		}
		~Parser() {}

	public:
		void Parse();

	public:
		inline Node* GetRoot() const { return m_root; }
        inline String GetFilename() const { return m_lexer->GetFilename(); }

	private:
		Prelude::ErrorManager &GetErrorManager();
		void Eat(TokenType type);
		TokenType Peek();

	private: // Common
		Nodes::IdentifierNode* Id();
		Nodes::FunctionCall* FunctionCall(Node* callee);

	private: // Expression
		Node* Factor();
		Node* Term();
		Node* Expr();
		Node* Cond();
		Node* Ternary();
		Node* Assignment();

    private: // Checkers
        bool IsTypeCastAhead();

    private: // Helpers
        bool ParseGenericType();

    private: // Types
        Nodes::TypeNode* Type();
        Nodes::TypeNode* Type(Nodes::IdentifierNode* target);
        Nodes::TemplateGeneric* TmplGeneric();
        Nodes::TypeDfNode* TypeDfStatement();
        Nodes::CastNode* Cast(Nodes::TypeNode* typ);

	private: // Statements
		Node* Statement();
        Node* CompileTimeStatement();
		Node* IfElseStatement();
		Node* VariableDeclaration();
        Node* ReturnStatement();
        Nodes::FunctionDeclaration* FunctionSignature();
        Node* FunctionDeclaration();
        Node* WhileLoop();
        Node* ForLoop();
        Node* BreakStmt();
    private: // Directives
        Node* ImportStatement();
        Node* ExternStatement();

	private:
		ArenaAllocator<> m_arena;

		Nodes::ProgramNode* m_root;
		Lexer* m_lexer;
        std::vector<AST::LocationSpan> m_breaks;
	};
}

