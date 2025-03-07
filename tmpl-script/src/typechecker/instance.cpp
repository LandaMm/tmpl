
#include "include/interpreter/value.h"
#include "include/typechecker.h"

namespace Runtime
{
    PValType TypeChecker::DiagnoseInstance(std::shared_ptr<InstanceNode> instance)
    {
        if (!m_type_definitions->HasItem(instance->GetTarget()->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), instance->GetTarget()->GetName(), instance->GetTarget()->GetLocation(), "TypeError");
            ReportError();
            return std::make_shared<ValType>(instance->GetTarget()->GetName());
        }

        auto typeDf = m_type_definitions->LookUp(instance->GetTarget()->GetName());
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
            errManager.TypeConstructorDoesNotExist(GetFilename(), std::make_shared<ValType>(instance->GetTarget()->GetName()), instance->GetTarget()->GetLocation(), "TypeError");
            ReportError();
            return std::make_shared<ValType>(instance->GetTarget()->GetName());
        }

        auto fn = typeDf->GetConstructor();
        assert(fn != nullptr && "Constructor fn should not be nullptr here.");

        auto fnCall = instance->GetFunctionCall();
        auto fnName = instance->GetTarget()->GetName() + "::constructor";

        auto retType = ResolveFn(fn, fnName, fnCall);

        return std::make_shared<ValType>(instance->GetTarget()->GetName());
    }
}

