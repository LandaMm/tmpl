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
		Parser(Lexer* lexer);
		~Parser();

	public:
		void Parse();

	public:
		inline Node* GetRoot() const { return m_root; }
        inline String GetFilename() { return CurrentLexer()->GetFilename(); }

	private:
		Prelude::ErrorManager &GetErrorManager();
		void PushLexer(Lexer* lexer);
		[[nodiscard]] Lexer* CurrentLexer();
		[[nodiscard]] Token* Current();
		[[nodiscard]] Token* Peek();
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
		Array<Lexer*> m_lexers;
		Array<Lexer*> m_deadLexers;
        std::vector<AST::LocationSpan> m_breaks;
	};
}

