
#include "include/interpreter.h"
#include "include/node/identifier.h"
#include "include/typechecker.h"

namespace Runtime
{
    void Interpreter::EvaluateCastDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        auto retType = TypeChecker::EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "RuntimeError", nullptr);
        auto fn = EvaluateFnDeclarationBasics(fnDecl, exported, externed);
        fn->SetReturnType(retType);

        assert(fnDecl->GetName()->GetType() == AST::NodeType::Identifier && "Name should be a type template by construct fn");
        // TODO: support generics
        auto id = std::dynamic_pointer_cast<AST::Nodes::IdentifierNode>(fnDecl->GetName());

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
}

