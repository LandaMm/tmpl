
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

        auto typFn = std::make_shared<TypeFn>(targetType, fn->GetModuleName(), fn->IsExported(), fn->GetLocation());

        auto it = std::make_shared<Common::Iterator>(fn->GetParamsSize());
        while (it->HasItems())
        {
            auto param = fn->GetItem(it->GetPosition());
            it->Next();
            typFn->AddParam(param);
        }

        return ResolveFn(typFn, fnName, fnCall);
    }
}

