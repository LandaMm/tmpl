

#include "../../include/typechecker.h"

namespace Runtime
{
    using namespace AST::Nodes;

    ValueType TypeChecker::DiagnoseFnCall(std::shared_ptr<FunctionCall> fnCall)
    {
        // TODO: support for object member calls
        assert(fnCall->GetCallee()->GetType() == NodeType::Identifier
                && "Unimplemented fn call callee node type");

        std::shared_ptr<IdentifierNode> callee =
            std::dynamic_pointer_cast<IdentifierNode>(fnCall->GetCallee());

        std::string fnName = callee->GetName();

        if (!m_functions->HasItem(fnName))
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.UndeclaredFunction(GetFilename(), callee, "TypeError");
            ReportError();
            return ValueType::Null;
        }

        std::shared_ptr<TypeFn> fn = m_functions->LookUp(fnName);

        auto args = fnCall->GetArgs();

        if (fn->GetParamsSize() != args->size())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.ArgsParamsExhausted(
                    GetFilename(),
                    fnName,
                    args->size(),
                    fn->GetParamsSize(),
                    fnCall->GetLocation(), "TypeError");
            ReportError();
            return fn->GetReturnType();
        }

        while (fn->HasParams())
        {
            std::shared_ptr<Node> arg = (*args)[fn->GetParamsIndex()];
            std::shared_ptr<FnParam> param = fn->GetNextParam();
            ValueType valType = DiagnoseNode(arg);
            if (valType != param->GetType())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.ArgMismatchType(
                        GetFilename(),
                        param->GetName(),
                        valType,
                        param->GetType(),
                        arg->GetLocation(),
                        "TypeError"
                        );
                ReportError();
                break; // will get to return of the fn's return type (+4 lines)
            }
        }

        fn->ResetIterator();

        return fn->GetReturnType();
    }
}


