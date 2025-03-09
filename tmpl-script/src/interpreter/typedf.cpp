
#include "include/interpreter.h"
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    void Interpreter::EvaluateTypeDefinition(std::shared_ptr<TypeDfNode> typeDfn, bool exported)
    {
        // check if type already exists
        auto typeName = typeDfn->GetTypeName();

        if (m_type_definitions->HasItem(typeName->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeRedeclaration(GetFilename(), typeName, "RuntimeError");
            return;
        }

        auto baseTypeNode = typeDfn->GetTypeValue();

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "RuntimeError", nullptr);
        genHandler.DefineGenerics(typeDfn->GetGenIterator(), false);

        auto baseTyp = TypeChecker::EvaluateType(GetFilename(), baseTypeNode, m_type_definitions, "RuntimeError", nullptr);

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

