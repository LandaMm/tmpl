
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
        std::shared_ptr<ComplexValueType> m_type;
        bool m_editable;
    public:
        TypeVariable(std::shared_ptr<ComplexValueType> type, bool editable)
            : m_type(type), m_editable(editable) { }
    public:
        inline std::shared_ptr<ComplexValueType> GetType() const { return m_type; }
        inline bool IsEditable() const { return m_editable; }
    };

	class TypeFn
	{
	private:
        std::vector<std::shared_ptr<FnParam>> m_params;
        std::shared_ptr<ComplexValueType> m_ret_type;
        std::string m_module_name;
        bool m_exported;
        Location m_loc;

	public:
		TypeFn(std::shared_ptr<ComplexValueType> retType, std::string module, bool exported, Location loc)
			: m_ret_type(retType),
            m_params(std::vector<std::shared_ptr<FnParam>>()),
            m_module_name(module),
            m_exported(exported),
            m_loc(loc) {}

    public:
        void AddParam(std::shared_ptr<FnParam> param) { m_params.push_back(param); }

	public:
        inline std::shared_ptr<FnParam> GetItem(unsigned int index) const
            { return m_params[index]; }
        inline size_t GetParamsSize() const { return m_params.size(); }
    public:
        inline std::shared_ptr<ComplexValueType> GetReturnType() const { return m_ret_type; }
        inline std::string GetModuleName() const { return m_module_name; }
        inline bool IsExported() const { return m_exported; }
        inline Location GetLocation() const { return m_loc; }
	};

    class TypeChecker
    {
    private:
        std::shared_ptr<Parser> m_parser;
        std::string m_filename;
        std::shared_ptr<Environment<TypeVariable>> m_variables;
        std::shared_ptr<Environment<TypeFn>> m_functions;
        std::shared_ptr<Environment<std::string>> m_modules;
        std::shared_ptr<Environment<Environment<TypeFn>, std::shared_ptr<ComplexValueType>>> m_type_functions;
        int m_errors;
        
    public:
        TypeChecker(std::shared_ptr<Parser> parser)
            : m_parser(parser), m_filename(parser->GetFilename()),
              m_variables(std::make_shared<Environment<TypeVariable>>()),
              m_functions(std::make_shared<Environment<TypeFn>>()),
              m_modules(std::make_shared<Environment<std::string>>()),
              m_type_functions(std::make_shared<Environment<Environment<TypeFn>, std::shared_ptr<ComplexValueType>>>()),
              m_errors(0) { }
    public:
        void RunChecker(std::shared_ptr<ProgramNode> program);
        void RunModuleChecker(std::shared_ptr<RequireMacro> require);
    private:
        std::shared_ptr<ComplexValueType> DiagnoseNode(std::shared_ptr<Node> node); // done
        std::shared_ptr<ComplexValueType> DiagnoseExpression(std::shared_ptr<ExpressionNode> expr); // done
        std::shared_ptr<ComplexValueType> DiagnoseLiteral(std::shared_ptr<LiteralNode> literal); // done
        std::shared_ptr<ComplexValueType> DiagnoseId(std::shared_ptr<IdentifierNode> identifier); // done
        std::shared_ptr<ComplexValueType> DiagnoseFnCall(std::shared_ptr<FunctionCall> fnCall); // done
        std::shared_ptr<ComplexValueType> DiagnoseUnary(std::shared_ptr<UnaryNode> unary); // done
        std::shared_ptr<ComplexValueType> DiagnoseCondition(std::shared_ptr<Condition> condition); // done
        std::shared_ptr<ComplexValueType> DiagnoseTernary(std::shared_ptr<TernaryNode> ternary); // done
        // TODO:

    private:
        void HandleVarDeclaration(std::shared_ptr<VarDeclaration> varDecl); // done
        void HandleFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported); // done
        void HandleFnSignature(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported); // done
        void HandleModule(std::shared_ptr<ProgramNode> program); // skip
        void HandleExportStatement(std::shared_ptr<ExportStatement> exportStmt); // skip

    private:
        void AssumeBlock(std::shared_ptr<Statements::StatementsBody> body, std::shared_ptr<ComplexValueType> expected); // done
        void AssumeIfElse(std::shared_ptr<Statements::IfElseStatement> ifElse, std::shared_ptr<ComplexValueType> expected); // done

    private:
        void ReportError() { m_errors++; };

    public:
        int GetErrorReport() { return m_errors; }

	public:
		static std::shared_ptr<ComplexValueType> EvaluateType(std::string filename, std::shared_ptr<Node> typeNode); // done

    private:
        std::string GetFilename() const { return m_filename; }
        void SetFilename(std::string filename) { m_filename = filename; }
    };
}

#endif // TYPECHECKER_H

