#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <memory>
#include "node/program.h"
#include "node/return.h"
#include "parser.h"
#include "node.h"
#include "node/expression.h"
#include "node/literal.h"
#include "node/var_declaration.h"
#include "node/identifier.h"
#include "node/logical.h"
#include "node/procedure.h"
#include "node/function.h"
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

	public:
        Interpreter(std::shared_ptr<Parser> parser,
                std::shared_ptr<Environment<Variable>> env_vars,
                std::shared_ptr<Environment<Procedure>> env_procedures,
                std::shared_ptr<Environment<Fn>> env_functions)
            : m_parser(parser),
            m_variables(env_vars),
            m_procedures(env_procedures),
            m_functions(env_functions) { }

	public:
		std::shared_ptr<Value> Execute(std::shared_ptr<Node> node);
        void Evaluate(std::shared_ptr<ProgramNode> program);

	private:
		std::shared_ptr<Value> EvaluateExpression(std::shared_ptr<ExpressionNode> expr);
		std::shared_ptr<Value> EvaluateLiteral(std::shared_ptr<LiteralNode> literal);
		std::shared_ptr<Value> EvaluateIdentifier(std::shared_ptr<IdentifierNode> identifier);
		std::shared_ptr<Value> EvaluateCondition(std::shared_ptr<Condition> condition);
		std::shared_ptr<Value> EvaluateTernary(std::shared_ptr<TernaryNode> ternary);
        std::shared_ptr<Value> EvaluateReturn(std::shared_ptr<ReturnNode> ret);
        std::shared_ptr<Value> EvaluateFunctionCall(std::shared_ptr<FunctionCall> ret);

	public:
		void EvaluateVariableDeclaration(std::shared_ptr<VarDeclaration> varDecl);
		void EvaluateProcedureDeclaration(std::shared_ptr<ProcedureDeclaration> procDecl);
        void EvaluateFunctionDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl);

	public:
		ValueType EvaluateType(std::shared_ptr<Node> typeNode);
	};
}

#endif
