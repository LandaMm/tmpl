

#include <memory>
#include "../../include/interpreter.h"
#include "../../include/node/statement.h"

namespace Runtime
{
    using namespace AST::Nodes;

    void Interpreter::EvaluateFunctionDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl)
    {
        std::string name = fnDecl->GetName();
        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();
        ValueType retType = EvaluateType(fnDecl->GetReturnType());

        std::shared_ptr<Fn> fn = std::make_shared<Fn>(body, retType);

        while (fnDecl->HasParams())
        {
            FunctionParam param = fnDecl->GetNextParam();
            ValueType paramType = EvaluateType(param.GetType());
            std::string paramName = param.GetName()->GetName();
            FnParam fnParam = FnParam(paramType, paramName);
            fn->AddParam(fnParam);
        }

        m_functions->AddItem(name, fn);
    }
}

