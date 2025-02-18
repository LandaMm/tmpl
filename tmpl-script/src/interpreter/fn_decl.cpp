

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
        PValType retType = TypeChecker::EvaluateType(GetFilename(), fnDecl->GetReturnType());

        std::shared_ptr<Fn> fn = std::make_shared<Fn>(body, retType, GetFilename(), exported, externed, fnDecl->GetLocation());

        auto it = std::make_shared<Common::Iterator>(fnDecl->GetSize());
        while (it->HasItems())
        {
            auto param = fnDecl->GetItem(it->GetPosition());
            it->Next();
            PValType paramType = TypeChecker::EvaluateType(GetFilename(), param->GetType());
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
        }

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

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
                assert(obj->GetObject()->GetType() == NodeType::Type && "Object target should've been typenode for type function (interpreter)");
                PValType targetType = TypeChecker::EvaluateType(GetFilename(), std::dynamic_pointer_cast<TypeNode>(obj->GetObject()));
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
                    errManager.FunctionRedeclaration(GetFilename(), fnName, fnDecl->GetLocation(), existingFn->GetModuleName(), existingFn->GetLocation(), "TypeError");
                    return;
                }

                typeEnv->AddItem(fnName, fn);
                break;
            }
            default:
            {
                Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
                errManager.RaiseError("Unsupported node for fn declaration", "TypeError");
                return;
            }
        }
    }
}

