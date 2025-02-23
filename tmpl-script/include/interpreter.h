#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <memory>
#include "include/node/assign.h"
#include "include/node/instance.h"
#include "include/node/loop.h"
#include "include/node/statement.h"
#include "include/node/type.h"
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
#include "typechecker/typedf.h"

namespace Runtime
{
	using namespace AST::Nodes;
	class Interpreter
	{
    public:
        using TypeDfs = Environment<TypeDf>;
        using PTypeDfs = std::shared_ptr<Environment<TypeDf>>;
	private:
		std::shared_ptr<Parser> m_parser;
		std::shared_ptr<Environment<Variable>> m_variables;
		std::shared_ptr<Environment<Procedure>> m_procedures;
		std::shared_ptr<Environment<Fn>> m_functions;

        std::shared_ptr<Environment<Environment<Fn>>> m_type_functions;
        PTypeDfs m_type_definitions;

        std::shared_ptr<Environment<std::string>> m_modules;

        std::shared_ptr<Environment<void*>> m_handles;

        std::string m_filename;

	public:
        Interpreter(std::shared_ptr<Parser> parser,
                std::shared_ptr<Environment<Variable>> env_vars,
                std::shared_ptr<Environment<Procedure>> env_procedures,
                std::shared_ptr<Environment<Fn>> env_functions,
                std::shared_ptr<Environment<std::string>> env_modules,
                std::shared_ptr<Environment<Environment<Fn>>> env_type_functions,
                PTypeDfs env_type_definitions)
            : m_parser(parser),
            m_variables(env_vars),
            m_procedures(env_procedures),
            m_functions(env_functions),
            m_type_functions(env_type_functions),
            m_type_definitions(env_type_definitions),
            m_modules(env_modules),
            m_handles(std::make_shared<Environment<void*>>()),
            m_filename(parser->GetFilename()) { }
        ~Interpreter();

    private:
        void CloseHandle(std::string handleKey);

	public:
		std::shared_ptr<Value> Execute(std::shared_ptr<Node> node); // DONE
        void Evaluate(std::shared_ptr<ProgramNode> program); // DONE
        void ImportModule(std::shared_ptr<RequireMacro> require); // DONE
        void EvaluateModule(std::shared_ptr<ProgramNode> program); // DONE

	private:
		std::shared_ptr<Value> EvaluateExpression(std::shared_ptr<ExpressionNode> expr); // DONE
		std::shared_ptr<Value> EvaluateLiteral(std::shared_ptr<LiteralNode> literal); // DONE
		std::shared_ptr<Value> EvaluateIdentifier(std::shared_ptr<IdentifierNode> identifier);
		std::shared_ptr<Value> EvaluateCondition(std::shared_ptr<Condition> condition); // DONE
		std::shared_ptr<Value> EvaluateTernary(std::shared_ptr<TernaryNode> ternary); // DONE
        std::shared_ptr<Value> EvaluateUnary(std::shared_ptr<UnaryNode> unary); // DONE
        std::shared_ptr<Value> EvaluateReturn(std::shared_ptr<ReturnNode> ret); // DONE
        std::shared_ptr<Value> EvaluateFunctionCall(std::shared_ptr<FunctionCall> ret); // DONE
        std::shared_ptr<Value> EvaluateIfElseStatement(std::shared_ptr<Statements::IfElseStatement> ifElse); // DONE
        std::shared_ptr<Value> EvaluateWhileLoop(std::shared_ptr<WhileNode> whileNode); // DONE
        std::shared_ptr<Value> EvaluateExternFunctionCall(std::string fnName, std::shared_ptr<Fn> fn, std::vector<std::shared_ptr<Node>>* args); // DONE
        std::shared_ptr<Value> EvaluateInstance(std::shared_ptr<InstanceNode> instance); // DONE
        std::shared_ptr<Value> EvaluateTypeCasting(std::shared_ptr<CastNode> cast); // DONE

    private:
        std::shared_ptr<Value> CastValue(std::shared_ptr<Value> val, PValType to, Location loc);

	private:
		void EvaluateVariableDeclaration(std::shared_ptr<VarDeclaration> varDecl); // DONE
		void EvaluateProcedureDeclaration(std::shared_ptr<ProcedureDeclaration> procDecl); // DONE
        void EvaluateFunctionDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed); // DONE
        void EvaluateExportStatement(std::shared_ptr<ExportStatement> exportStmt); // DONE
        void EvaluateTypeDefinition(std::shared_ptr<TypeDfNode> typeDfn, bool exported);
        void EvaluateAssignment(std::shared_ptr<AssignmentNode> assignment);

    private:
        inline std::string GetFilename() const { return m_filename; }
        void SetFilename(std::string filename) { m_filename = filename; }
	};
}

#endif
