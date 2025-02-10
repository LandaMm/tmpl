

#include "../../include/typechecker.h"
#include "../../include/iterator.h"
#include "../../include/node.h"
#include "../../include/node/identifier.h"
#include "../../include/node/object_member.h"
#include "include/interpreter/environment.h"
#include <error.h>
#include <memory>

namespace Runtime
{
    using namespace AST::Nodes;

    void TypeChecker::HandleFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();
        ValueType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType());

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        std::shared_ptr<TypeFn> fn = std::make_shared<TypeFn>(retType, GetFilename(), exported, fnDecl->GetLocation());

        auto currentScope = m_variables;
        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);

        auto it = std::make_shared<Common::Iterator>(fnDecl->GetSize());
        while (it->HasItems())
        {
            auto param = fnDecl->GetItem(it->GetPosition());
            it->Next();
            ValueType paramType = EvaluateType(GetFilename(), param->GetType());
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
            auto typeVar = std::make_shared<TypeVariable>(paramType, false);
            variables->AddItem(paramName, typeVar);
        }

        switch(nameNode->GetType())
        {
            case AST::NodeType::Identifier:
            {
                std::string name = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

                m_functions->AddItem(name, fn);
                break;
            }
            case AST::NodeType::ObjectMember:
            {
                auto obj = std::dynamic_pointer_cast<ObjectMember>(nameNode);
                ValueType targetType = EvaluateType(GetFilename(), obj->GetObject());
                if (!m_type_functions->HasItem(targetType))
                    m_type_functions->AddItem(targetType, std::make_shared<Environment<TypeFn>>());

                std::shared_ptr<Environment<TypeFn>> typeEnv = m_type_functions->LookUp(targetType);
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
                    return;
                }

                typeEnv->AddItem(fnName, fn);

                auto typeVar = std::make_shared<TypeVariable>(targetType, false);
                variables->AddItem("self", typeVar);
                break;
            }
            default:
            {
                Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
                errManager.RaiseError("Unsupported node for fn declaration", "TypeError");
                return;
            }
        }

        m_variables = variables;

        AssumeBlock(body, retType);

        m_variables = currentScope;
    }

    void TypeChecker::HandleFnSignature(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        ValueType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType());

        std::shared_ptr<Node> nameNode = fnDecl->GetName();
        assert(nameNode->GetType() == NodeType::Identifier && "Name should be identifier");
        std::string name = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

        std::shared_ptr<TypeFn> fn = std::make_shared<TypeFn>(retType, GetFilename(), exported, fnDecl->GetLocation());

        auto it = std::make_shared<Common::Iterator>(fnDecl->GetSize());
        while (it->HasItems())
        {
            auto param = fnDecl->GetItem(it->GetPosition());
            it->Next();
            ValueType paramType = EvaluateType(GetFilename(), param->GetType());
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
            auto typeVar = std::make_shared<TypeVariable>(paramType, false);
        }

        m_functions->AddItem(name, fn);
    }
}

