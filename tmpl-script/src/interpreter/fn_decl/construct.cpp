
#include "include/interpreter.h"
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    void Interpreter::EvaluateConstructDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        assert(fnDecl->GetModifier() == AST::Nodes::FunctionModifier::Construct && "Construct declaration should have appropriate modifier");

        assert(fnDecl->GetName()->GetType() == AST::NodeType::Identifier && "Name should be a type template by construct fn");

        auto fn = EvaluateFnDeclarationBasics(fnDecl, exported, externed);

        std::shared_ptr<Node> nameNode = fnDecl->GetName();
        assert(nameNode->GetType() == AST::NodeType::Identifier && "Name should be a type template by construct fn");

        std::string typeName = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

        if (!m_type_definitions->HasItem(typeName))
        {
            Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), typeName, nameNode->GetLocation(), "RuntimeError");
            return;
        }

        auto typeDf = m_type_definitions->LookUp(typeName);
        assert(typeDf != nullptr && "Type df shouldn't be null at this point.");

        if (GetFilename() != typeDf->GetModuleName() && !typeDf->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateTypeError(GetFilename(), typeDf->GetTypeName(), typeDf->GetModuleName(), fnDecl->GetName()->GetLocation(), typeDf->GetLocation(), "RuntimeError");
            return;
        }

        if (typeDf->HasConstructor())
        {
            Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeConstructorRedeclaration(GetFilename(), typeName, fnDecl->GetLocation(), "RuntimeError");
            return;
        }

        if (typeDf->GenericsSize() != fn->GetGenericsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.TypeGenericsExhausted(GetFilename(), typeName, fn->GetGenericsSize(), typeDf->GenericsSize(), nameNode->GetLocation(), "RuntimeError");
            return;
        }

        auto it = Common::Iterator(typeDf->GenericsSize());
        while (it.HasItems())
        {
            auto tpDfGen = typeDf->GetGeneric(it.GetPosition());
            auto tmplGen = fn->GetGeneric(it.GetPosition());
            if (tpDfGen->GetName() != tmplGen->GetName())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.TypeGenericNameMismatch(GetFilename(), typeName, tmplGen->GetName(), it.GetPosition(), tmplGen->GetLocation(), "RuntimeError");
                return;
            }
            it.Next();
        }

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "RuntimeError", nullptr);
        genHandler.DefineGenerics(fn->GetGenIterator(), false);

        auto baseType = typeDf->GetBaseType();

        fn->SetReturnType(baseType);

        typeDf->SetConstructor(fn);

        m_type_definitions = genHandler.Unload();
    }
}


