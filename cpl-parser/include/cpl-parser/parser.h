#pragma once

#include "cpl-parser/node/cast.hpp"
#include "cpl-parser/lexer.h"
#include "cpl-parser/node.h"
#include "cpl-parser/node/program.hpp"
#include "cpl-parser/node/identifier.hpp"
#include "cpl-parser/node/function.hpp"
#include "cpl-parser/node/type.hpp"
#include "cpl-parser/error.h"
#include "cpl-basics/allocator/arena.hpp"
#include "cpl-basics/def.hpp"
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
		[[nodiscard]] Token* Current() const noexcept;
		[[nodiscard]] TokenType Peek();
		void Advance();
		void Eat(TokenType type);

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
		Uint64 EvaluateIntegerConstantExpression(Node* node) const;

    private: // Types
        Nodes::Type* Type();
        Nodes::TypeDeclaration* TypeDeclaration();
        Nodes::CastNode* Cast(Nodes::Type* typ);

	private: // Statements
		Node* Statement();
        Node* CompileTimeStatement();
		Node* IfElseStatement();
		Node* VariableDeclaration();
        Node* ReturnStatement();
        Nodes::FunctionDeclaration* FunctionSignature();
		Nodes::FunctionDeclaration *FunctionDeclaration();
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

