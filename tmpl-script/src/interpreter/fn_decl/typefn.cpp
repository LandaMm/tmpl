
#include "include/interpreter.h"
#include "include/iterator.h"
#include "include/node/identifier.h"
#include "include/typechecker.h"

namespace Runtime
{
    void Interpreter::EvaluateTypeFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        assert(fnDecl->GetName()->GetType() == NodeType::ObjectMember && "Function name should be an object member for type function");

        auto fn = EvaluateFnDeclarationBasics(fnDecl, exported, externed);

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "RuntimeError", nullptr);
        genHandler.DefineGenerics(fn->GetGenIterator(), false);

        auto retType = TypeChecker::EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "RuntimeError", nullptr);

        fn->SetReturnType(retType);
        m_type_definitions = genHandler.Unload();

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        auto obj = std::dynamic_pointer_cast<ObjectMember>(nameNode);
        std::cout << "[INFO] type function object target: " << obj->GetObject()->Format() << std::endl; 
        assert(obj->GetObject()->GetType() == NodeType::Identifier && "Object target should be a type for type fn");

        auto typTmpl = std::dynamic_pointer_cast<IdentifierNode>(obj->GetObject());
        auto typeName = typTmpl->GetName();

        if (!m_type_definitions->HasItem(typeName))
        {
            Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), typeName, typTmpl->GetLocation(), "RuntimeError");
            return;
        }

        auto typeDf = m_type_definitions->LookUp(typeName);
        assert(typeDf != nullptr && "Type df shouldn't be null at this point.");

        if (GetFilename() != typeDf->GetModuleName() && !typeDf->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateTypeError(GetFilename(), typeDf->GetTypeName(), typeDf->GetModuleName(), nameNode->GetLocation(), typeDf->GetLocation(), "RuntimeError");
            return;
        }

        if (typeDf->GenericsSize() != fn->GetGenericsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.TypeGenericsExhausted(GetFilename(), typeName, fn->GetGenericsSize(), typeDf->GenericsSize(), typTmpl->GetLocation(), "RuntimeError");
            return;
        }

        auto gIt = Common::Iterator(typeDf->GenericsSize());
        while (gIt.HasItems())
        {
            auto tpDfGen = typeDf->GetGeneric(gIt.GetPosition());
            auto tmplGen = fn->GetGeneric(gIt.GetPosition());
            if (tpDfGen->GetName() != tmplGen->GetName())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.TypeGenericNameMismatch(GetFilename(), typeName, tmplGen->GetName(), gIt.GetPosition(), tmplGen->GetLocation(), "RuntimeError");
                return;
            }
            gIt.Next();
        }

        auto typeEnv = typeDf->GetTypFnsEnv();
        assert(typeEnv != nullptr && "Type env should have been created.");

        std::shared_ptr<Node> fnNameNode = obj->GetMember();
        std::string fnName = std::dynamic_pointer_cast<IdentifierNode>(fnNameNode)->GetName();
        // TODO: think if "Contains" is better in that case
        if (typeEnv->HasItem(fnName))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            auto existingFn = typeEnv->LookUp(fnName);
            assert(existingFn != nullptr && "Existing fn should not be nullptr");
            errManager.FunctionRedeclaration(GetFilename(), fnName, fnDecl->GetLocation(), existingFn->GetModuleName(), existingFn->GetLocation(), "RuntimeError");
            return;
        }

        typeEnv->AddItem(fnName, fn);
    }
}

