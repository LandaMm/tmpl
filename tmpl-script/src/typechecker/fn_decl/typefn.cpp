
#include "include/iterator.h"
#include "include/node/identifier.h"
#include "include/typechecker.h"

namespace Runtime
{
    void TypeChecker::HandleTypeFunctionDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        assert(fnDecl->GetName()->GetType() == NodeType::ObjectMember && "Function name should be an object member for type function");

        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();
        PValType baseType = nullptr;

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        auto fn = HandleFnBasics(fnDecl, exported);

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "TypeError", this);
        genHandler.DefineGenerics(fn->GetGenIterator(), false);

        PValType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "TypeError", this);
        fn->SetReturnType(retType);

        m_type_definitions = genHandler.Unload();

        // Assuming

        auto currentScope = m_variables;
        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);

        auto aGenHandler = GenHandler(GetFilename(), m_type_definitions, "TypeError", this);
        aGenHandler.DefineGenerics(fn->GetGenIterator(), true);

        auto aIt = std::make_shared<Common::Iterator>(fnDecl->GetParamsSize());
        while (aIt->HasItems())
        {
            auto param = fn->GetParam(aIt->GetPosition());
            PValType paramType = NormalizeType(GetFilename(), param->GetType(), fnDecl->GetParam(aIt->GetPosition())->GetType()->GetLocation(), m_type_definitions, "TypeError", this);
            std::string paramName = param->GetName();
            auto typeVar = std::make_shared<TypeVariable>(paramType, false);
            variables->AddItem(paramName, typeVar);
            aIt->Next();
        }

        auto obj = std::dynamic_pointer_cast<ObjectMember>(nameNode);
        assert(obj->GetObject()->GetType() == NodeType::Identifier && "Object target should be a type for type fn");

        auto typTmpl = std::dynamic_pointer_cast<IdentifierNode>(obj->GetObject());
        auto typeName = typTmpl->GetName();

        if (!m_type_definitions->HasItem(typeName))
        {
            Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), typeName, typTmpl->GetLocation(), "TypeError");
            ReportError();
            return;
        }

        auto typeDf = m_type_definitions->LookUp(typeName);
        assert(typeDf != nullptr && "Type df shouldn't be null at this point.");

        if (GetFilename() != typeDf->GetModuleName() && !typeDf->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateTypeError(GetFilename(), typeDf->GetTypeName(), typeDf->GetModuleName(), nameNode->GetLocation(), typeDf->GetLocation(), "TypeError");
            ReportError();
            return;
        }

        if (typeDf->GenericsSize() != fn->GetGenericsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.TypeGenericsExhausted(GetFilename(), typeName, fn->GetGenericsSize(), typeDf->GenericsSize(), typTmpl->GetLocation(), "TypeError");
            ReportError();
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
                errorManager.TypeGenericNameMismatch(GetFilename(), typeName, tmplGen->GetName(), gIt.GetPosition(), tmplGen->GetLocation(), "TypeError");
                ReportError();
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
            errManager.FunctionRedeclaration(GetFilename(), fnName, fnDecl->GetLocation(), existingFn->GetModuleName(), existingFn->GetLocation(), "TypeError");
            ReportError();
            return;
        }

        typeEnv->AddItem(fnName, fn);

        // TODO: valtype with generics
        auto valType = std::make_shared<ValType>(typeName);

        auto vIt = Common::Iterator(fn->GetGenericsSize());
        while (vIt.HasItems())
        {
            auto gen = fn->GetGeneric(vIt.GetPosition());
            valType->AddGeneric(std::make_shared<ValType>(gen->GetName()));
            vIt.Next();
        }

        auto typeVar = std::make_shared<TypeVariable>(valType, false);
        variables->AddItem("self", typeVar);

        m_variables = variables;

        AssumeBlock(body, retType);

        m_variables = currentScope;

        m_type_definitions = aGenHandler.Unload();
    }
}

