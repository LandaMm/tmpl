

#include "../../include/typechecker.h"
#include "../../include/iterator.h"
#include "../../include/node.h"
#include "../../include/node/identifier.h"
#include "../../include/node/object_member.h"
#include "include/interpreter/environment.h"
#include "include/interpreter/value.h"
#include "include/node/function.h"
#include "include/node/type.h"
#include "include/typechecker/typedf.h"
#include "include/error.h"
#include <memory>

namespace Runtime
{
    using namespace AST::Nodes;

    void TypeChecker::HandleFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();
        PValType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType());
        PValType baseType;

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        std::shared_ptr<TypeFn> fn = std::make_shared<TypeFn>(retType, GetFilename(), exported, fnDecl->GetLocation());

        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);
        auto typDefinitions = std::make_shared<Environment<Runtime::TypeDf>>(m_type_definitions);

        auto tIt = std::make_shared<Common::Iterator>(fnDecl->GetGenericsSize());
        while (tIt->HasItems())
        {
            auto generic = fnDecl->GetGeneric(tIt->GetPosition());
            tIt->Next();
            std::string name = generic->GetName()->GetName();
            auto fnGeneric = std::make_shared<FnGeneric>(name);
            fn->AddGeneric(fnGeneric);

            auto dummBaseTyp = std::make_shared<ValType>("void");
            auto typDf = std::make_shared<Runtime::TypeDf>(name, dummBaseTyp, GetFilename(), false, generic->GetLocation());
            typDefinitions->AddItem(name, typDf);
        }

        m_type_definitions = typDefinitions;

        auto it = std::make_shared<Common::Iterator>(fnDecl->GetParamsSize());
        while (it->HasItems())
        {
            auto param = fnDecl->GetParam(it->GetPosition());
            it->Next();
            PValType paramType = EvaluateType(GetFilename(), param->GetType());
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
            auto typeVar = std::make_shared<TypeVariable>(paramType, false);
            variables->AddItem(paramName, typeVar);
        }

        switch(fnDecl->GetModifier())
        {
            case AST::Nodes::FunctionModifier::Construct:
            {
                assert(nameNode->GetType() == AST::NodeType::Identifier && "Name should be a type template by construct fn");
                auto id = std::dynamic_pointer_cast<AST::Nodes::IdentifierNode>(nameNode);

                std::string typeName = id->GetName();

                if (!m_type_definitions->HasItem(typeName))
                {
                    Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
                    errManager.TypeDoesNotExist(GetFilename(), typeName, id->GetLocation(), "TypeError");
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

                // TODO: compare generics amount and names
                if (typeDf->HasConstructor())
                {
                    Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
                    errManager.TypeConstructorRedeclaration(GetFilename(), typeName, fnDecl->GetLocation(), "TypeError");
                    ReportError();
                    return;
                }

                baseType = typeDf->GetBaseType();

                auto constructorFn =
                    std::make_shared<Fn>(nullptr, baseType, GetFilename(), exported, false, fnDecl->GetLocation());

                auto it = std::make_shared<Common::Iterator>(fn->GetParamsSize());
                while (it->HasItems())
                {
                    auto param = fn->GetParam(it->GetPosition());
                    it->Next();
                    constructorFn->AddParam(param);
                }

                typeDf->SetConstructor(constructorFn);

                break;
            }
            case AST::Nodes::FunctionModifier::Cast:
            {
                assert(nameNode->GetType() == AST::NodeType::TypeTemplate && "Name should be a type template by construct fn");
                // TODO: support generics
                auto id = std::dynamic_pointer_cast<AST::Nodes::TypeTemplateNode>(nameNode);

                std::string typeName = id->GetTypeName()->GetName();

                if (!m_type_definitions->HasItem(typeName))
                {
                    Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
                    errManager.TypeDoesNotExist(GetFilename(), typeName, id->GetLocation(), "TypeError");
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

                auto casts = typeDf->GetCastsEnv();

                // TODO: compare generics amount and names
                if (casts->HasItem(retType->GetName()))
                {
                    Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
                    errManager.TypeCastRedeclaration(GetFilename(), typeName, retType, fnDecl->GetLocation(), "TypeError");
                    ReportError();
                    return;
                }

                auto castFn =
                    std::make_shared<Fn>(nullptr, retType, GetFilename(), exported, false, fnDecl->GetLocation());

                auto typDfCast =
                    std::make_shared<TypeDfCast>(typeName, retType->GetName(), castFn);

                casts->AddItem(retType->GetName(), typDfCast);

                // TODO: generics support
                auto dummTypNode =
                    std::make_shared<TypeNode>(id->GetTypeName(), id->GetTypeName()->GetLocation());
                PValType targetType = EvaluateType(GetFilename(), dummTypNode);

                auto typeVar = std::make_shared<TypeVariable>(targetType, false);
                variables->AddItem("self", typeVar);

                break;
            }
            default:
                // skip, no additional actions required
                break;
        }

        switch(nameNode->GetType())
        {
            case AST::NodeType::Identifier:
            {
                std::string name = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

                if (fnDecl->GetModifier() == FunctionModifier::None)
                    m_functions->AddItem(name, fn);
                break;
            }
            case AST::NodeType::ObjectMember:
            {
                assert(fnDecl->GetModifier() != FunctionModifier::Cast && "Object members are not supported in cast functions as names.");
                assert(fnDecl->GetModifier() != FunctionModifier::Construct && "Object members are not supported in cast functions as names.");

                auto obj = std::dynamic_pointer_cast<ObjectMember>(nameNode);
                assert(obj->GetObject()->GetType() == NodeType::Type && "Object target should be a type for type fn");
                PValType targetType = EvaluateType(GetFilename(), std::dynamic_pointer_cast<TypeNode>(obj->GetObject()));
                if (!m_type_functions->HasItem(targetType->GetName()))
                    m_type_functions->AddItem(targetType->GetName(), std::make_shared<Environment<TypeFn>>());

                std::shared_ptr<Environment<TypeFn>> typeEnv = m_type_functions->LookUp(targetType->GetName());
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

                auto typeVar = std::make_shared<TypeVariable>(targetType, false);
                variables->AddItem("self", typeVar);
                break;
            }
            case AST::NodeType::TypeTemplate:
                // skip (cast function)
                break;
            default:
            {
                Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
                errManager.RaiseError("Unsupported node for fn declaration: " + nameNode->Format(), "TypeError");
                ReportError();
                return;
            }
        }

        m_variables = variables;

        if (fnDecl->GetModifier() == AST::Nodes::FunctionModifier::Construct)
        {
            assert(baseType != nullptr && "Base type should have been set.");
            AssumeBlock(body, baseType);
        }
        else
            AssumeBlock(body, retType);

        assert(variables->GetParent() != nullptr && "Variables parent gone.");
        m_variables = variables->GetParent();
        assert(m_variables != variables && "Parent and child are located at the same memory space.");

        assert(typDefinitions->GetParent() != nullptr && "Type Definitions parent gone.");
        m_type_definitions = typDefinitions->GetParent();
        assert(m_type_definitions != typDefinitions && "Parent and child are located at the same memory space.");
    }

    void TypeChecker::HandleFnSignature(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        PValType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType());

        std::shared_ptr<Node> nameNode = fnDecl->GetName();
        assert(nameNode->GetType() == NodeType::Identifier && "Name should be identifier");
        std::string name = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

        std::shared_ptr<TypeFn> fn = std::make_shared<TypeFn>(retType, GetFilename(), exported, fnDecl->GetLocation());

        auto it = std::make_shared<Common::Iterator>(fnDecl->GetParamsSize());
        while (it->HasItems())
        {
            auto param = fnDecl->GetParam(it->GetPosition());
            it->Next();
            PValType paramType = EvaluateType(GetFilename(), param->GetType());
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
            auto typeVar = std::make_shared<TypeVariable>(paramType, false);
        }

        m_functions->AddItem(name, fn);
    }
}

