

#include "../../include/typechecker.h"
#include "include/interpreter/value.h"
#include "include/iterator.h"
#include "include/node/function.h"
#include <memory>

namespace Runtime
{
    using namespace AST::Nodes;

    PValType TypeChecker::ResolveFn(std::shared_ptr<TypeFn> fn, std::string fnName, std::shared_ptr<FunctionCall> fnCall)
    {
        if (GetFilename() != fn->GetModuleName() && !fn->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateFunctionError(GetFilename(), fnName, fn->GetModuleName(), fnCall->GetLocation(), fn->GetLocation(), "TypeError");
            ReportError();
            return std::make_shared<ValType>("void");
        }

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

        auto it = std::make_shared<Common::Iterator>(fn->GetParamsSize());
        while (it->HasItems())
        {
            auto param = fn->GetItem(it->GetPosition());
            std::shared_ptr<Node> arg = (*args)[it->GetPosition()];
            it->Next();
            PValType valType = DiagnoseNode(arg);
            if (!valType->Compare(*param->GetType()))
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

        return fn->GetReturnType();
    }

    bool TypeChecker::ResolveTypeFn(std::shared_ptr<ObjectMember> obj, std::shared_ptr<TypeFn>& fn, std::string& fnName)
    {
        PValType targetType = DiagnoseNode(obj->GetObject());
        if (!m_type_functions->HasItem(targetType->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.UndeclaredFunction(GetFilename(), obj, targetType, "TypeError");
            ReportError();
            return false;
        }

        std::shared_ptr<Environment<TypeFn>> typeEnv = m_type_functions->LookUp(targetType->GetName());
        assert(typeEnv != nullptr && "Type env should have been created.");

        std::shared_ptr<Node> fnNameNode = obj->GetMember();
        fnName = std::dynamic_pointer_cast<IdentifierNode>(fnNameNode)->GetName();
        // TODO: think if "Contains" is better in that case
        if (!typeEnv->HasItem(fnName))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.UndeclaredFunction(GetFilename(), obj, targetType, "TypeError");
            ReportError();
            return false;
        }

        fn = typeEnv->LookUp(fnName);
        return true;
    }

    PValType TypeChecker::DiagnoseFnCall(std::shared_ptr<FunctionCall> fnCall)
    {
        // TODO: support for object member calls
        std::shared_ptr<Node> callee = fnCall->GetCallee();

        std::shared_ptr<TypeFn> fn;
        std::string fnName;

        switch(callee->GetType())
        {
            case AST::NodeType::ObjectMember:
            {
                if (!ResolveTypeFn(std::dynamic_pointer_cast<ObjectMember>(callee), fn, fnName))
                {
                    // encourented an error
                    return std::make_shared<ValType>("void");
                }
                break;
            }
            case AST::NodeType::Identifier:
            {
                auto idCallee = std::dynamic_pointer_cast<IdentifierNode>(callee);
                fnName = idCallee->GetName();

                if (!m_functions->HasItem(fnName))
                {
                    Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                    errorManager.UndeclaredFunction(GetFilename(), idCallee, "TypeError");
                    ReportError();
                    return std::make_shared<ValType>("void");
                }

                fn = m_functions->LookUp(fnName);
                break;
            }
            default:
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.RaiseError("Invalid node for function call", "TypeError");
                ReportError();
                return std::make_shared<ValType>("void");
            }
        }

        return ResolveFn(fn, fnName, fnCall);
    }
}


