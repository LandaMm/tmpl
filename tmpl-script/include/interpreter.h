#ifndef INTERPRETER_H
#define INTERPRETER_H
#include<memory>
#include"parser.h"
#include"node.h"
#include"node/expression.h"
#include"node/literal.h"
#include"node/var_declaration.h"
#include"interpreter/environment.h"
#include"interpreter/value.h"

namespace Runtime
{
	using namespace AST::Nodes;
	class Interpreter
	{
	private:
		std::shared_ptr<Parser> m_parser;
		std::shared_ptr<Environment> m_env;
	public:
		Interpreter(std::shared_ptr<Parser> parser, std::shared_ptr<Environment> env) : m_parser(parser), m_env(env) {}
	public:
		std::shared_ptr<Value> Evaluate(std::shared_ptr<Node> node);
		std::shared_ptr<Value> EvaluateExpression(std::shared_ptr<ExpressionNode> expr);
		void EvaluateVariableDeclaration(std::shared_ptr<VarDeclaration> varDecl);
		std::shared_ptr<Value> EvaluateLiteral(std::shared_ptr<LiteralNode> literal);
		ValueType EvaluateType(std::shared_ptr<Node> typeNode);
	};
}

#endif