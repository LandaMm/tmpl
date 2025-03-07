
#include "include/typechecker/typedf.h"
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    void TypeChecker::HandleTypeDefinition(std::shared_ptr<TypeDfNode> typeDfn, bool exported)
    {
        // check if type already exists
        TypeDfNode::PId typeName = typeDfn->GetTypeName();

        if (m_type_definitions->HasItem(typeName->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeRedeclaration(GetFilename(), typeName, "TypeError");
            ReportError();
            return;
        }

        auto baseTypeNode = typeDfn->GetTypeValue();

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "TypeError", this);
        genHandler.DefineGenerics(typeDfn->GetGenIterator(), false);

        auto baseTyp = EvaluateType(GetFilename(), baseTypeNode, m_type_definitions, "TypeError", this);

        m_type_definitions = genHandler.Unload();

        std::string key = typeName->GetName();
        auto typeDf = std::make_shared<TypeDf>(key, baseTyp, GetFilename(), exported, typeDfn->GetLocation());

        auto gIt = Common::Iterator(typeDfn->GetGenericsSize());
        while (gIt.HasItems())
        {
            auto gen = typeDfn->GetGeneric(gIt.GetPosition());
            gIt.Next();

            auto typDfGen = std::make_shared<TypeDfGeneric>(gen->GetName(), gen->GetLocation());
            typeDf->AddGeneric(typDfGen);
        }

        m_type_definitions->AddItem(key, typeDf);
    }
}

