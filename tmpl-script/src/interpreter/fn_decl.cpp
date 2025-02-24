

#include <memory>
#include "../../include/interpreter.h"
#include "../../include/node/statement.h"
#include "../../include/typechecker.h"
#include "include/iterator.h"
#include "include/node/function.h"
#include "include/node/object_member.h"

namespace Runtime
{
    using namespace AST::Nodes;

    void Interpreter::EvaluateFunctionDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();
        PValType retType = nullptr;

        if (fnDecl->GetModifier() != AST::Nodes::FunctionModifier::Construct)
        {
            retType = TypeChecker::EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "RuntimeError", nullptr);
        }

        std::shared_ptr<Fn> fn = std::make_shared<Fn>(body, retType, GetFilename(), exported, externed, fnDecl->GetLocation());

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        auto it = std::make_shared<Common::Iterator>(fnDecl->GetSize());
        while (it->HasItems())
        {
            auto param = fnDecl->GetItem(it->GetPosition());
            it->Next();
            PValType paramType = TypeChecker::EvaluateType(GetFilename(), param->GetType(), m_type_definitions, "RuntimeError", nullptr);
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
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
                    errManager.TypeDoesNotExist(GetFilename(), typeName, id->GetLocation(), "RuntimeError");
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

                // TODO: compare generics amount and names
                if (typeDf->HasConstructor())
                {
                    Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
                    errManager.TypeConstructorRedeclaration(GetFilename(), typeName, fnDecl->GetLocation(), "RuntimeError");
                    return;
                }

                auto baseType = typeDf->GetBaseType();

                fn->SetReturnType(baseType);

                typeDf->SetConstructor(fn);

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
                    errManager.TypeDoesNotExist(GetFilename(), typeName, id->GetLocation(), "RuntimeError");
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
                auto obj = std::dynamic_pointer_cast<ObjectMember>(nameNode);
                assert(obj->GetObject()->GetType() == NodeType::Type && "Object target should've been typenode for type function (interpreter)");
                PValType targetType = TypeChecker::EvaluateType(GetFilename(), std::dynamic_pointer_cast<TypeNode>(obj->GetObject()), m_type_definitions, "RuntimeError", nullptr);
                if (!m_type_functions->HasItem(targetType->GetName()))
                    m_type_functions->AddItem(targetType->GetName(), std::make_shared<Environment<Fn>>());

                std::shared_ptr<Environment<Fn>> typeEnv = m_type_functions->LookUp(targetType->GetName());
                assert(typeEnv != nullptr && "Type env should have been created.");

                std::shared_ptr<Node> fnNameNode = obj->GetMember();
                assert(fnNameNode->GetType() == NodeType::Identifier && "Type function name should be an identifier node.");
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
                break;
            }
            case AST::NodeType::TypeTemplate:
                // skip (cast function)
                break;
            default:
            {
                Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
                errManager.RaiseError("Unsupported node for fn declaration", "RuntimeError");
                return;
            }
        }
    }
}

