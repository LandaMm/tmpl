#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <memory>
#include "include/node/statement.h"
#include "node/return.h"
#include "node/program.h"
#include "parser.h"
#include "node.h"
#include "node/literal.h"
#include "node/var_declaration.h"
#include "node/identifier.h"
#include "node/logical.h"
#include "node/procedure.h"
#include "node/function.h"
#include "node/macros.h"
#include "node/export.h"
#include "node/unary.h"
#include "interpreter/environment.h"
#include "interpreter/value.h"

namespace Runtime
{
	using namespace AST::Nodes;
	class Interpreter
	{
	private:
		std::shared_ptr<Parser> m_parser;
		std::shared_ptr<Environment<Variable>> m_variables;
		std::shared_ptr<Environment<Procedure>> m_procedures;
		std::shared_ptr<Environment<Fn>> m_functions;

        std::string m_filename;

	public:
        Interpreter(std::shared_ptr<Parser> parser,
                std::shared_ptr<Environment<Variable>> env_vars,
                std::shared_ptr<Environment<Procedure>> env_procedures,
                std::shared_ptr<Environment<Fn>> env_functions)
            : m_parser(parser),
            m_variables(env_vars),
            m_procedures(env_procedures),
            m_functions(env_functions),
            m_filename(parser->GetFilename()) { }

	public:
		std::shared_ptr<Value> Execute(std::shared_ptr<Node> node);
        void Evaluate(std::shared_ptr<ProgramNode> program);
        void ImportModule(std::shared_ptr<RequireMacro> require);
        void EvaluateModule(std::shared_ptr<ProgramNode> program);

	private:
		std::shared_ptr<Value> EvaluateExpression(std::shared_ptr<ExpressionNode> expr);
		std::shared_ptr<Value> EvaluateLiteral(std::shared_ptr<LiteralNode> literal);
		std::shared_ptr<Value> EvaluateIdentifier(std::shared_ptr<IdentifierNode> identifier);
		std::shared_ptr<Value> EvaluateCondition(std::shared_ptr<Condition> condition);
		std::shared_ptr<Value> EvaluateTernary(std::shared_ptr<TernaryNode> ternary);
        std::shared_ptr<Value> EvaluateUnary(std::shared_ptr<UnaryNode> unary);
        std::shared_ptr<Value> EvaluateReturn(std::shared_ptr<ReturnNode> ret);
        std::shared_ptr<Value> EvaluateFunctionCall(std::shared_ptr<FunctionCall> ret);
        std::shared_ptr<Value> EvaluateIfElseStatement(std::shared_ptr<Statements::IfElseStatement> ifElse);

	private:
		void EvaluateVariableDeclaration(std::shared_ptr<VarDeclaration> varDecl);
		void EvaluateProcedureDeclaration(std::shared_ptr<ProcedureDeclaration> procDecl);
        void EvaluateFunctionDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl);
        void EvaluateExportStatement(std::shared_ptr<ExportStatement> exportStmt);

    private:
        inline std::string GetFilename() const { return m_filename; }
        void SetFilename(std::string filename) { m_filename = filename; }
	};
}

#endif
