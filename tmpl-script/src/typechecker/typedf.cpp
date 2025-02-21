
#include "include/typechecker.h"

namespace Runtime
{
    void TypeChecker::HandleTypeDefinition(std::shared_ptr<TypeDfNode> typeDfn, bool exported)
    {
        // check if type already exists
        auto typeName = typeDfn->GetTypeTemplate();

        if (m_type_definitions->HasItem(typeName->GetTypeName()->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeRedeclaration(GetFilename(), typeName, "TypeError");
            ReportError();
            return;
        }

        auto baseType = typeDfn->GetTypeValue();

        std::string key = typeName->GetTypeName()->GetName();
        auto typeDf = std::make_shared<TypeDf>(key, EvaluateType(GetFilename(), baseType), GetFilename(), exported, typeDfn->GetLocation());

        m_type_definitions->AddItem(key, typeDf);
    }
}

