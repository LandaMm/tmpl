

#include "../../include/typechecker.h"

namespace Runtime
{
    using namespace AST::Nodes;

    void TypeChecker::HandleFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();
        ValueType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType());

        std::string name = fnDecl->GetName();

        std::shared_ptr<TypeFn> fn = std::make_shared<TypeFn>(retType, GetFilename(), exported, fnDecl->GetLocation());

        auto currentScope = m_variables;
        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);

        fnDecl->ResetIterator();
        while (fnDecl->HasParams())
        {
            std::shared_ptr<FunctionParam> param = fnDecl->GetNextParam();
            ValueType paramType = EvaluateType(GetFilename(), param->GetType());
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
            auto typeVar = std::make_shared<TypeVariable>(paramType, false);
            variables->AddItem(paramName, typeVar);
        }
        fnDecl->ResetIterator();

        m_functions->AddItem(name, fn);

        m_variables = variables;

        AssumeBlock(body, retType);

        m_variables = currentScope;
    }

    void TypeChecker::HandleFnSignature(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        ValueType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType());

        std::string name = fnDecl->GetName();

        std::shared_ptr<TypeFn> fn = std::make_shared<TypeFn>(retType, GetFilename(), exported, fnDecl->GetLocation());

        auto currentScope = m_variables;
        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);

        fnDecl->ResetIterator();
        while (fnDecl->HasParams())
        {
            std::shared_ptr<FunctionParam> param = fnDecl->GetNextParam();
            ValueType paramType = EvaluateType(GetFilename(), param->GetType());
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
            auto typeVar = std::make_shared<TypeVariable>(paramType, false);
            variables->AddItem(paramName, typeVar);
        }
        fnDecl->ResetIterator();

        m_functions->AddItem(name, fn);
    }
}

