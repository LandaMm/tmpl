
#include "include/typechecker/typedf.h"
#include "include/iterator.h"
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

        auto baseTypeNode = typeDfn->GetTypeValue();

        m_type_definitions = DefineTemplateTypes(GetFilename(), typeName, m_type_definitions, false, "TypeError", this);

        auto baseTyp = EvaluateType(GetFilename(), baseTypeNode, m_type_definitions, "TypeError", this);

        assert(m_type_definitions->GetParent() != nullptr && "Typ dfs parent gone.");
        assert(m_type_definitions->GetParent() != m_type_definitions && "Parent clone");
        m_type_definitions = m_type_definitions->GetParent();

        std::string key = typeName->GetTypeName()->GetName();
        auto typeDf = std::make_shared<TypeDf>(key, baseTyp, GetFilename(), exported, typeDfn->GetLocation());

        auto gIt = Common::Iterator(typeName->GetGenericsSize());
        while (gIt.HasItems())
        {
            auto gen = typeName->GetTemplateGeneric(gIt.GetPosition());
            gIt.Next();

            auto typDfGen = std::make_shared<TypeDfGeneric>(gen->GetName());
            typeDf->AddGeneric(typDfGen);
        }

        m_type_definitions->AddItem(key, typeDf);
    }
}

