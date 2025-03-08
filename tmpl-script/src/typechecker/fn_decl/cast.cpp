
#include "include/interpreter/environment.h"
#include "include/iterator.h"
#include "include/node/identifier.h"
#include "include/typechecker.h"

namespace Runtime
{
    void TypeChecker::HandleCastDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();

        PValType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "TypeError", this);

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        auto fn = HandleFnBasics(fnDecl, exported);
        fn->SetReturnType(retType);

        auto currentScope = m_variables;
        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);

        // Assuming

        auto aIt = std::make_shared<Common::Iterator>(fnDecl->GetParamsSize());
        while (aIt->HasItems())
        {
            auto param = fnDecl->GetParam(aIt->GetPosition());
            PValType paramType = EvaluateType(GetFilename(), param->GetType(), m_type_definitions, "TypeError", this);
            std::string paramName = param->GetName()->GetName();
            auto typeVar = std::make_shared<TypeVariable>(paramType, false);
            variables->AddItem(paramName, typeVar);
            aIt->Next();
        }

        assert(nameNode->GetType() == AST::NodeType::Identifier && "Name should be a type template by construct fn");
        // TODO: support generics
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
            std::make_shared<Fn>(nullptr, retType, FnModifier::Cast, GetFilename(), exported, false, fnDecl->GetLocation());

        auto typDfCast =
            std::make_shared<TypeDfCast>(typeName, retType->GetName(), castFn);

        casts->AddItem(retType->GetName(), typDfCast);

        // TODO: generics support
        auto dummTypNode =
            std::make_shared<TypeNode>(id, id->GetLocation());
        PValType targetType = EvaluateType(GetFilename(), dummTypNode, m_type_definitions, "TypeError", this);

        auto typeVar = std::make_shared<TypeVariable>(targetType, false);
        variables->AddItem("self", typeVar);

        m_variables = variables;

        AssumeBlock(body, retType);

        m_variables = currentScope;
    }
}

