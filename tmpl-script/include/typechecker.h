
#ifndef TYPECHECKER_H
#define TYPECHECKER_H
#include <memory>
#include "include/interpreter.h"
#include "include/node/logical.h"
#include "include/node/unary.h"
#include "interpreter/environment.h"
#include "interpreter/value.h"
#include "parser.h"
#include "node/program.h"
#include "node/expression.h"
#include "node/literal.h"
#include "node/identifier.h"
#include "node/statement.h"
#include "node/var_declaration.h"
#include "node/function.h"

namespace Runtime
{
    using namespace AST::Nodes;

    class TypeVariable
    {
    private:
        ValueType m_type;
        bool m_editable;
    public:
        TypeVariable(ValueType type, bool editable)
            : m_type(type), m_editable(editable) { }
    public:
        inline ValueType GetType() const { return m_type; }
        inline bool IsEditable() const { return m_editable; }
    };

	class TypeFn
	{
	private:
        std::vector<std::shared_ptr<FnParam>> m_params;
        ValueType m_ret_type;
    private:
        size_t m_index;

	public:
		TypeFn(ValueType retType)
			: m_ret_type(retType), m_params(std::vector<std::shared_ptr<FnParam>>()), m_index(0) {}

    public:
        void AddParam(std::shared_ptr<FnParam> param) { m_params.push_back(param); }

	public:
        inline bool HasParams() { return m_index < m_params.size(); }
        inline std::shared_ptr<FnParam> GetNextParam() { return m_params[m_index++]; }
        inline size_t GetParamsSize() { return m_params.size(); }
        inline size_t GetParamsIndex() { return m_index; }
        inline ValueType GetReturnType() { return m_ret_type; }
	};

    class TypeChecker
    {
    private:
        std::shared_ptr<Parser> m_parser;
        std::string m_filename;
        std::shared_ptr<Environment<TypeVariable>> m_variables;
        std::shared_ptr<Environment<TypeFn>> m_functions;
        
    public:
        TypeChecker(std::shared_ptr<Parser> parser)
            : m_parser(parser), m_filename(parser->GetFilename()),
              m_variables(std::make_shared<Environment<TypeVariable>>()),
              m_functions(std::make_shared<Environment<TypeFn>>()) { }
    public:
        void RunChecker(std::shared_ptr<ProgramNode> program);
        void RunModuleChecker(std::shared_ptr<RequireMacro> require);
    private:
        ValueType DiagnoseNode(std::shared_ptr<Node> node);
        ValueType DiagnoseExpression(std::shared_ptr<ExpressionNode> expr);
        ValueType DiagnoseLiteral(std::shared_ptr<LiteralNode> literal);
        ValueType DiagnoseId(std::shared_ptr<IdentifierNode> identifier);
        ValueType DiagnoseFnCall(std::shared_ptr<FunctionCall> fnCall);
        ValueType DiagnoseUnary(std::shared_ptr<UnaryNode> unary);
        ValueType DiagnoseCondition(std::shared_ptr<Condition> condition);
        ValueType DiagnoseTernary(std::shared_ptr<TernaryNode> ternary);
        // TODO:

    private:
        void HandleVarDeclaration(std::shared_ptr<VarDeclaration> varDecl);
        void HandleFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl);
        void HandleModule(std::shared_ptr<ProgramNode> program);
        void HandleExportStatement(std::shared_ptr<ExportStatement> exportStmt);

    private:
        void AssumeBlock(std::shared_ptr<Statements::StatementsBody> body, ValueType expected);
        void AssumeIfElse(std::shared_ptr<Statements::IfElseStatement> ifElse, ValueType expected);

	public:
		static ValueType EvaluateType(std::string filename, std::shared_ptr<Node> typeNode);

    private:
        std::string GetFilename() const { return m_filename; }
        void SetFilename(std::string filename) { m_filename = filename; }
    };
}

#endif // TYPECHECKER_H

