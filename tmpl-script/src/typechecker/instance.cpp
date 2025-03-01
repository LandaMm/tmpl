
#include "include/interpreter/value.h"
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    PValType TypeChecker::DiagnoseInstance(std::shared_ptr<InstanceNode> instance)
    {
        PValType targetType = EvaluateType(GetFilename(), instance->GetTarget(), m_type_definitions, "TypeError", this);

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

        auto typDfs = std::make_shared<TypeDfs>(m_type_definitions);
        m_type_definitions = typDfs;

        auto gIt = Common::Iterator(typeDf->GenericsSize());
        while (gIt.HasItems())
        {
            auto gen = typeDf->GetGeneric(gIt.GetPosition());
            auto genType = EvaluateType(GetFilename(), instance->GetTarget()->GetGeneric(gIt.GetPosition()), m_type_definitions, "TypeError", this);
            // TODO: check for base and default type
            auto typDf = std::make_shared<TypeDf>(gen->GetName(), genType, GetFilename(), false, instance->GetTarget()->GetGeneric(gIt.GetPosition())->GetLocation());
            typDf->SetTransparent(true);
            typDfs->AddItem(gen->GetName(), typDf);
            gIt.Next();
        }

        auto fn = typeDf->GetConstructor();
        assert(fn != nullptr && "Constructor fn should not be nullptr here.");

        auto fnCall = instance->GetFunctionCall();
        auto fnName = targetType->GetName() + "::constructor";

        auto retType = ResolveFn(fn, fnName, fnCall);

        assert(m_type_definitions->GetParent() != nullptr && "Parent gone.");
        assert(m_type_definitions->GetParent() != m_type_definitions && "Typ df parent clone");
        m_type_definitions = m_type_definitions->GetParent();

        return retType;
    }
}

