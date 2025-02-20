
#include "include/interpreter/value.h"
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    PValType TypeChecker::DiagnoseInstance(std::shared_ptr<InstanceNode> instance)
    {
        PValType targetType = EvaluateType(GetFilename(), instance->GetTarget());

        if (!m_type_definitions->HasItem(targetType->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), targetType, instance->GetTarget()->GetLocation(), "TypeError");
            ReportError();
            return targetType;
        }

        auto typeDf = m_type_definitions->LookUp(targetType->GetName());
        assert(typeDf != nullptr && "Type definition should not be not found at this point.");

        if (GetFilename() != typeDf->GetModuleName() && !typeDf->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateTypeError(GetFilename(), typeDf->GetTypeName(), typeDf->GetModuleName(), instance->GetLocation(), typeDf->GetLocation(), "TypeError");
            ReportError();
            return std::make_shared<ValType>("void");
        }

        if (!typeDf->HasConstructor())
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeConstructorDoesNotExist(GetFilename(), targetType, instance->GetTarget()->GetLocation(), "TypeError");
            ReportError();
            return targetType;
        }

        auto fn = typeDf->GetConstructor();
        assert(fn != nullptr && "Constructor fn should not be nullptr here.");

        auto fnCall = instance->GetFunctionCall();
        auto fnName = targetType->GetName() + "::constructor";

        if (GetFilename() != fn->GetModuleName() && !fn->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateFunctionError(GetFilename(), fnName, fn->GetModuleName(), fnCall->GetLocation(), fn->GetLocation(), "TypeError");
            ReportError();
            return targetType;
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
            return targetType;
        }

        auto it = std::make_shared<Common::Iterator>(fn->GetParamsSize());
        while (it->HasItems())
        {
            auto param = fn->GetItem(it->GetPosition());
            std::shared_ptr<Node> arg = (*args)[it->GetPosition()];
            it->Next();
            PValType val = DiagnoseNode(arg);
            if (!val->Compare(*param->GetType()))
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.ArgMismatchType(
                        GetFilename(),
                        param->GetName(),
                        val,
                        param->GetType(),
                        arg->GetLocation(),
                        "TypeError"
                        );
                ReportError();
                return targetType;
            }
        }

        return targetType;
    }
}

