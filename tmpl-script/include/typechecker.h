
#ifndef TYPECHECKER_H
#define TYPECHECKER_H
#include <memory>
#include "include/helper.h"
#include "include/interpreter.h"
#include "include/node/assign.h"
#include "include/node/cast.h"
#include "include/node/instance.h"
#include "include/node/list.h"
#include "include/node/logical.h"
#include "include/node/loop.h"
#include "include/node/type.h"
#include "include/node/unary.h"
#include "include/typechecker/typedf.h"
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
#include "node/type.h"

namespace Runtime
{
    using namespace AST::Nodes;

    class TypeVariable
    {
    private:
        PValType m_type;
        bool m_editable;
    public:
        TypeVariable(PValType type, bool editable)
            : m_type(type), m_editable(editable) { }
    public:
        inline PValType GetType() const { return m_type; }
        inline bool IsEditable() const { return m_editable; }
    };

	class TypeFn
	{
	private:
        std::vector<std::shared_ptr<FnParam>> m_params;
        PValType m_ret_type;
        std::string m_module_name;
        bool m_exported;
        Location m_loc;

	public:
		TypeFn(PValType retType, std::string module, bool exported, Location loc)
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
        inline PValType GetReturnType() const { return m_ret_type; }
        inline std::string GetModuleName() const { return m_module_name; }
        inline bool IsExported() const { return m_exported; }
        inline Location GetLocation() const { return m_loc; }
	};

    class TypeChecker
    {
    public:
        using TypeDfs = Environment<TypeDf>;
        using PTypeDfs = std::shared_ptr<Environment<TypeDf>>;
    private:
        std::shared_ptr<Parser> m_parser;
        std::string m_filename;
        std::shared_ptr<Environment<TypeVariable>> m_variables;
        std::shared_ptr<Environment<TypeFn>> m_functions;
        std::shared_ptr<Environment<std::string>> m_modules;
        // TODO: move inside type_definition
        std::shared_ptr<Environment<Environment<TypeFn>>> m_type_functions;
        PTypeDfs m_type_definitions;
        int m_errors;
        
    public:
        TypeChecker(std::shared_ptr<Parser> parser)
            : m_parser(parser), m_filename(parser->GetFilename()),
              m_variables(std::make_shared<Environment<TypeVariable>>()),
              m_functions(std::make_shared<Environment<TypeFn>>()),
              m_modules(std::make_shared<Environment<std::string>>()),
              m_type_functions(std::make_shared<Environment<Environment<TypeFn>>>()),
              m_type_definitions(Helper::Helper::GetTypeDefinitions()),
              m_errors(0) { }
    public:
        void RunChecker(std::shared_ptr<ProgramNode> program); // DONE
        void RunModuleChecker(std::shared_ptr<RequireMacro> require); // DONE
    private:
        PValType DiagnoseNode(std::shared_ptr<Node> node); // DONE
        PValType DiagnoseExpression(std::shared_ptr<ExpressionNode> expr); // DONE
        PValType DiagnoseLiteral(std::shared_ptr<LiteralNode> literal); // DONE
        PValType DiagnoseId(std::shared_ptr<IdentifierNode> identifier); // DONE
        PValType DiagnoseFnCall(std::shared_ptr<FunctionCall> fnCall); // DONE
        PValType DiagnoseUnary(std::shared_ptr<UnaryNode> unary); // DONE
        PValType DiagnoseCondition(std::shared_ptr<Condition> condition); // DONE
        PValType DiagnoseTernary(std::shared_ptr<TernaryNode> ternary); // DONE
        PValType DiagnoseInstance(std::shared_ptr<InstanceNode> instance);
        PValType DiagnoseTypeCasting(std::shared_ptr<CastNode> cast);
        PValType DiagnoseList(std::shared_ptr<ListNode> list);

    private:
        void HandleVarDeclaration(std::shared_ptr<VarDeclaration> varDecl); // DONE
        void HandleFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported); // DONE
        void HandleFnSignature(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported); // DONE
        void HandleModule(std::shared_ptr<ProgramNode> program); // DONE
        void HandleExportStatement(std::shared_ptr<ExportStatement> exportStmt); // DONE
        void HandleTypeDefinition(std::shared_ptr<TypeDfNode> typeDfn, bool exported);
        void HandleAssignment(std::shared_ptr<AssignmentNode> assignment);

    private:
        void AssumeBlock(std::shared_ptr<Statements::StatementsBody> body, PValType expected); // DONE
        void AssumeIfElse(std::shared_ptr<Statements::IfElseStatement> ifElse, PValType expected); // DONE
        void AssumeWhile(std::shared_ptr<WhileNode> whileNode, PValType expected);
        void AssumeForLoop(std::shared_ptr<ForLoopNode> forLoopNode, PValType expected);

    private:
        PValType ResolveFn(std::shared_ptr<TypeFn> fn, std::string fnName, std::shared_ptr<FunctionCall> fnCall);
        bool ResolveTypeFn(std::shared_ptr<ObjectMember> obj, std::shared_ptr<TypeFn>& fn, std::string& fnName);

    private:
        void ReportError() { m_errors++; };

    public:
        int GetErrorReport() { return m_errors; }

	public:
		static PValType EvaluateType(std::string filename, std::shared_ptr<TypeNode> typeNode, TypeChecker::PTypeDfs typeDfs, std::string prefix, TypeChecker* typChecker);
        static PValType CastType(std::string filename, PValType from, PValType to, Location loc, TypeChecker::PTypeDfs typeDfs, std::string prefix);
        static PValType GetRootType(std::string filename, PValType target, Location loc, TypeChecker::PTypeDfs typeDfs, std::string prefix);

    private:
        std::string GetFilename() const { return m_filename; }
        void SetFilename(std::string filename) { m_filename = filename; }
    };
}

#endif // TYPECHECKER_H

