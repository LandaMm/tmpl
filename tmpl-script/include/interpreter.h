#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <memory>
#include "parser.h"
#include "node.h"
#include "node/expression.h"
#include "node/literal.h"
#include "node/var_declaration.h"
#include "node/identifier.h"
#include "node/logical.h"
#include "node/procedure.h"
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

	public:
		Interpreter(std::shared_ptr<Parser> parser, std::shared_ptr<Environment<Variable>> env_vars, std::shared_ptr<Environment<Procedure>> env_procedures)
			: m_parser(parser), m_variables(env_vars), m_procedures(env_procedures) {}

	public:
		std::shared_ptr<Value> Evaluate(std::shared_ptr<Node> node);

	private:
		std::shared_ptr<Value> EvaluateExpression(std::shared_ptr<ExpressionNode> expr);
		std::shared_ptr<Value> EvaluateLiteral(std::shared_ptr<LiteralNode> literal);
		std::shared_ptr<Value> EvaluateIdentifier(std::shared_ptr<IdentifierNode> identifier);
		std::shared_ptr<Value> EvaluateCondition(std::shared_ptr<Condition> condition);
		std::shared_ptr<Value> EvaluateTernary(std::shared_ptr<TernaryNode> ternary);

	public:
		void EvaluateVariableDeclaration(std::shared_ptr<VarDeclaration> varDecl);
		void EvaluateProcedureDeclaration(std::shared_ptr<ProcedureDeclaration> procDecl);

	public:
		ValueType EvaluateType(std::shared_ptr<Node> typeNode);
	};
}

#endif