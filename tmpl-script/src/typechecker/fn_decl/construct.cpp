
#include "include/interpreter/value.h"
#include "include/iterator.h"
#include "include/node/identifier.h"
#include "include/typechecker.h"
#include <memory>

namespace Runtime
{
    void TypeChecker::HandleConstructDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        assert(fnDecl->GetModifier() == AST::Nodes::FunctionModifier::Construct && "Function declaration should have construct modifier");

        std::shared_ptr<Node> nameNode = fnDecl->GetName();
        assert(nameNode->GetType() == AST::NodeType::Identifier && "Name should be a type template by construct fn");

        std::string typeName = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

        auto fn = HandleFnBasics(fnDecl, exported);

        if (!m_type_definitions->HasItem(typeName))
        {
            Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), typeName, nameNode->GetLocation(), "TypeError");
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

        if (typeDf->HasConstructor())
        {
            Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeConstructorRedeclaration(GetFilename(), typeName, fnDecl->GetLocation(), "TypeError");
            ReportError();
            return;
        }

        if (typeDf->GenericsSize() != fn->GetGenericsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.TypeGenericsExhausted(GetFilename(), typeName, fn->GetGenericsSize(), typeDf->GenericsSize(), nameNode->GetLocation(), "TypeError");
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

        auto currentScope = m_variables;
        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);

        // Assuming

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "TypeError", this);
        genHandler.DefineGenerics(fn->GetGenIterator(), true);

        auto aIt = std::make_shared<Common::Iterator>(fn->GetParamsSize());
        while (aIt->HasItems())
        {
            auto param = fn->GetParam(aIt->GetPosition());
            /*PValType paramType = EvaluateType(GetFilename(), param->GetType(), m_type_definitions, "TypeError", this);*/
            PValType paramType = NormalizeType(GetFilename(), param->GetType(), fnDecl->GetParam(aIt->GetPosition())->GetType()->GetLocation(), m_type_definitions, "TypeError", this);
            std::string paramName = param->GetName();
            auto typeVar = std::make_shared<TypeVariable>(paramType, false);
            variables->AddItem(paramName, typeVar);
            aIt->Next();
        }

        PValType baseType = typeDf->GetBaseType();

        fn->SetReturnType(baseType);

        typeDf->SetConstructor(fn);

        m_variables = variables;

        assert(baseType != nullptr && "Base type should have been set.");
        AssumeBlock(fnDecl->GetBody(), baseType);

        m_type_definitions = genHandler.Unload();

        m_variables = currentScope;
    }
}

