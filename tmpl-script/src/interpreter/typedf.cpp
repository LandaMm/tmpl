
#include "include/interpreter.h"

namespace Runtime
{
    void Interpreter::EvaluateTypeDefinition(std::shared_ptr<TypeDfNode> typeDfn)
    {
        // check if type already exists
        auto typeName = typeDfn->GetTypeTemplate();

        if (m_type_definitions->HasItem(typeName->GetTypeName()->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeRedeclaration(GetFilename(), typeName, "RuntimeError");
            return;
        }

        auto baseType = typeDfn->GetTypeValue();

        std::string key = typeName->GetTypeName()->GetName();
        auto typeDf = std::make_shared<TypeDf>(key, baseType->GetTypeName()->GetName());

        m_type_definitions->AddItem(key, typeDf);
    }
}

