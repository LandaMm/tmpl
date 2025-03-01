

#include <memory>
#include "../../include/interpreter.h"
#include "../../include/typechecker.h"
#include "include/interpreter/value.h"
#include "include/iterator.h"
#include "include/node/function.h"
#include "include/node/object_member.h"

namespace Runtime
{
    using namespace AST::Nodes;

    std::shared_ptr<Fn> Interpreter::EvaluateFnDeclarationBasics(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed, PValType retType)
    {
        std::shared_ptr<Fn> fn = std::make_shared<Fn>(fnDecl->GetBody(), retType, GetFilename(), exported, externed, fnDecl->GetLocation());

        auto it = std::make_shared<Common::Iterator>(fnDecl->GetParamsSize());
        while (it->HasItems())
        {
            auto param = fnDecl->GetParam(it->GetPosition());
            it->Next();
            PValType paramType = TypeChecker::EvaluateType(GetFilename(), param->GetType(), m_type_definitions, "RuntimeError", nullptr);
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
        }

        return fn;
    }

    void Interpreter::EvaluateConstructDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        assert(fnDecl->GetModifier() == AST::Nodes::FunctionModifier::Construct && "Construct declaration should have appropriate modifier");

        assert(fnDecl->GetName()->GetType() == AST::NodeType::TypeTemplate && "Name should be a type template by construct fn");

        auto fn = EvaluateFnDeclarationBasics(fnDecl, exported, externed, nullptr);

        auto typTmpl = std::dynamic_pointer_cast<TypeTemplateNode>(fnDecl->GetName());
        m_type_definitions = TypeChecker::DefineTemplateTypes(GetFilename(), typTmpl, m_type_definitions, false, "RuntimeError", nullptr);

        std::string typeName = typTmpl->GetTypeName()->GetName();

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
            errorManager.PrivateTypeError(GetFilename(), typeDf->GetTypeName(), typeDf->GetModuleName(), fnDecl->GetName()->GetLocation(), typeDf->GetLocation(), "RuntimeError");
            return;
        }

        if (typeDf->GenericsSize() != typTmpl->GetGenericsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.TypeGenericsExhausted(GetFilename(), typeName, typTmpl->GetGenericsSize(), typeDf->GenericsSize(), typTmpl->GetLocation(), "RuntimeError");
            return;
        }

        auto it = Common::Iterator(typeDf->GenericsSize());
        while (it.HasItems())
        {
            auto tpDfGen = typeDf->GetGeneric(it.GetPosition());
            auto tmplGen = typTmpl->GetTemplateGeneric(it.GetPosition());
            if (tpDfGen->GetName() != tmplGen->GetName())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.TypeGenericNameMismatch(GetFilename(), typeName, tmplGen->GetName(), it.GetPosition(), tmplGen->GetLocation(), "RuntimeError");
                return;
            }
            it.Next();
        }

        if (typeDf->HasConstructor())
        {
            Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeConstructorRedeclaration(GetFilename(), typeName, fnDecl->GetLocation(), "RuntimeError");
            return;
        }

        auto baseType = typeDf->GetBaseType();

        fn->SetReturnType(baseType);

        typeDf->SetConstructor(fn);

        assert(m_type_definitions->GetParent() != nullptr && "Parent gone.");
        assert(m_type_definitions->GetParent() != m_type_definitions && "Typ df parent clone");
        m_type_definitions = m_type_definitions->GetParent();
    }

    void Interpreter::EvaluateCastDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        auto retType = TypeChecker::EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "RuntimeError", nullptr);
        auto fn = EvaluateFnDeclarationBasics(fnDecl, exported, externed, retType);

        assert(fnDecl->GetName()->GetType() == AST::NodeType::TypeTemplate && "Name should be a type template by construct fn");
        // TODO: support generics
        auto id = std::dynamic_pointer_cast<AST::Nodes::TypeTemplateNode>(fnDecl->GetName());

        std::string typeName = id->GetTypeName()->GetName();

        if (!m_type_definitions->HasItem(typeName))
        {
            Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), typeName, id->GetLocation(), "RuntimeError");
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

        auto casts = typeDf->GetCastsEnv();

        // TODO: compare generics amount and names
        if (casts->HasItem(retType->GetName()))
        {
            Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeCastRedeclaration(GetFilename(), typeName, retType, fnDecl->GetLocation(), "RuntimeError");
            return;
        }

        auto typDfCast =
            std::make_shared<TypeDfCast>(typeName, retType->GetName(), fn);

        casts->AddItem(retType->GetName(), typDfCast);
    }

    void Interpreter::EvaluateFunctionDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        switch(fnDecl->GetModifier())
        {
            case AST::Nodes::FunctionModifier::Construct:
                return EvaluateConstructDeclaration(fnDecl, exported, externed);
            case AST::Nodes::FunctionModifier::Cast:
                return EvaluateCastDeclaration(fnDecl, exported, externed);
            case AST::Nodes::FunctionModifier::None:
                if (fnDecl->GetName()->GetType() == NodeType::Identifier)
                    return EvaluateRegularFnDeclaration(fnDecl, exported, externed);
                else
                    return EvaluateTypeFnDeclaration(fnDecl, exported, externed);
        }

        assert(false && "Should handle every fn modifier");
    }

    void Interpreter::EvaluateTypeFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        assert(fnDecl->GetName()->GetType() == NodeType::ObjectMember && "Function name should be an object member for type function");

        auto retType = TypeChecker::EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "RuntimeError", nullptr);

        auto fn = EvaluateFnDeclarationBasics(fnDecl, exported, externed, retType);

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        auto obj = std::dynamic_pointer_cast<ObjectMember>(nameNode);
        std::cout << "[INFO] type function object target: " << obj->GetObject()->Format() << std::endl; 
        assert(obj->GetObject()->GetType() == NodeType::TypeTemplate && "Object target should be a type for type fn");

        auto typTmpl = std::dynamic_pointer_cast<TypeTemplateNode>(obj->GetObject());
        auto typeName = typTmpl->GetTypeName()->GetName();

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

        if (typeDf->GenericsSize() != typTmpl->GetGenericsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.TypeGenericsExhausted(GetFilename(), typeName, typTmpl->GetGenericsSize(), typeDf->GenericsSize(), typTmpl->GetLocation(), "RuntimeError");
            return;
        }

        auto gIt = Common::Iterator(typeDf->GenericsSize());
        while (gIt.HasItems())
        {
            auto tpDfGen = typeDf->GetGeneric(gIt.GetPosition());
            auto tmplGen = typTmpl->GetTemplateGeneric(gIt.GetPosition());
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

    void Interpreter::EvaluateRegularFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        auto retType = TypeChecker::EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "RuntimeError", nullptr);

        auto fn = EvaluateFnDeclarationBasics(fnDecl, exported, externed, retType);

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        std::string name = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

        if (fnDecl->GetModifier() == FunctionModifier::None)
            m_functions->AddItem(name, fn);
    }
}

