

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
#include <error.h>
#include <memory>

namespace Runtime
{
    using namespace AST::Nodes;

    void TypeChecker::HandleFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        switch(fnDecl->GetModifier())
        {
            case AST::Nodes::FunctionModifier::None:
                if (fnDecl->GetName()->GetType() == NodeType::Identifier)
                    return HandleRegularDeclaration(fnDecl, exported);
                else
                    return HandleTypeFunctionDeclaration(fnDecl, exported);
            case AST::Nodes::FunctionModifier::Cast:
                return HandleCastDeclaration(fnDecl, exported);
            case AST::Nodes::FunctionModifier::Construct:
                return HandleConstructDeclaration(fnDecl, exported);
        }

        assert(false && "Should handle all function modifiers");
    }

    std::shared_ptr<Fn> TypeChecker::HandleFnBasics(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, PValType retType)
    {
        std::shared_ptr<Fn> fn = std::make_shared<Fn>(fnDecl->GetBody(), retType, GetFilename(), exported, false, fnDecl->GetLocation());

        auto it = std::make_shared<Common::Iterator>(fnDecl->GetParamsSize());
        while (it->HasItems())
        {
            auto param = fnDecl->GetParam(it->GetPosition());
            it->Next();
            PValType paramType = EvaluateType(GetFilename(), param->GetType(), m_type_definitions, "TypeError", this);
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
        }

        return fn;
    }

    void TypeChecker::HandleConstructDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        assert(fnDecl->GetModifier() == AST::Nodes::FunctionModifier::Construct && "Function declaration should have construct modifier");

        std::shared_ptr<Node> nameNode = fnDecl->GetName();
        assert(nameNode->GetType() == AST::NodeType::TypeTemplate && "Name should be a type template by construct fn");

        auto typTmpl = std::dynamic_pointer_cast<TypeTemplateNode>(nameNode);

        std::string typeName = typTmpl->GetTypeName()->GetName();

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

        if (typeDf->GenericsSize() != typTmpl->GetGenericsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.TypeGenericsExhausted(GetFilename(), typeName, typTmpl->GetGenericsSize(), typeDf->GenericsSize(), typTmpl->GetLocation(), "TypeError");
            ReportError();
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
                errorManager.TypeGenericNameMismatch(GetFilename(), typeName, tmplGen->GetName(), gIt.GetPosition(), tmplGen->GetLocation(), "TypeError");
                ReportError();
                return;
            }
            gIt.Next();
        }

        if (typeDf->HasConstructor())
        {
            Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeConstructorRedeclaration(GetFilename(), typeName, fnDecl->GetLocation(), "TypeError");
            ReportError();
            return;
        }

        m_type_definitions = DefineTemplateTypes(GetFilename(), typTmpl, m_type_definitions, false, "TypeError", this);

        auto fn = HandleFnBasics(fnDecl, exported, nullptr);

        auto currentScope = m_variables;
        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);

        assert(m_type_definitions->GetParent() != nullptr && "Parent gone.");
        assert(m_type_definitions->GetParent() != m_type_definitions && "Typ df parent clone");
        m_type_definitions = m_type_definitions->GetParent();

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

        m_type_definitions = DefineTemplateTypes(GetFilename(), typTmpl, m_type_definitions, true, "TypeError", this);

        PValType baseType = typeDf->GetBaseType();

        auto constructorFn =
            std::make_shared<Fn>(nullptr, baseType, GetFilename(), exported, false, fnDecl->GetLocation());

        auto pIt = std::make_shared<Common::Iterator>(fn->GetParamsSize());
        while (pIt->HasItems())
        {
            auto param = fn->GetItem(pIt->GetPosition());
            pIt->Next();
            constructorFn->AddParam(param);
        }

        typeDf->SetConstructor(constructorFn);

        m_variables = variables;

        assert(baseType != nullptr && "Base type should have been set.");
        AssumeBlock(fnDecl->GetBody(), baseType);

        assert(m_type_definitions->GetParent() != nullptr && "Parent gone.");
        assert(m_type_definitions->GetParent() != m_type_definitions && "Typ df parent clone");
        m_type_definitions = m_type_definitions->GetParent();

        m_variables = currentScope;
    }

    void TypeChecker::HandleCastDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();

        PValType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "TypeError", this);

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        auto fn = HandleFnBasics(fnDecl, exported, retType);

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
        PValType targetType = EvaluateType(GetFilename(), dummTypNode, m_type_definitions, "TypeError", this);

        auto typeVar = std::make_shared<TypeVariable>(targetType, false);
        variables->AddItem("self", typeVar);

        m_variables = variables;

        AssumeBlock(body, retType);

        m_variables = currentScope;
    }

    void TypeChecker::HandleTypeFunctionDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        assert(fnDecl->GetName()->GetType() == NodeType::ObjectMember && "Function name should be an object member for type function");

        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();
        PValType baseType = nullptr;

        PValType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "TypeError", this);

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        auto fn = HandleFnBasics(fnDecl, exported, retType);

        auto currentScope = m_variables;
        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);

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

        auto obj = std::dynamic_pointer_cast<ObjectMember>(nameNode);
        std::cout << "[INFO] type function object target: " << obj->GetObject()->Format() << std::endl; 
        assert(obj->GetObject()->GetType() == NodeType::TypeTemplate && "Object target should be a type for type fn");

        auto typTmpl = std::dynamic_pointer_cast<TypeTemplateNode>(obj->GetObject());
        auto typeName = typTmpl->GetTypeName()->GetName();

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

        if (typeDf->GenericsSize() != typTmpl->GetGenericsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.TypeGenericsExhausted(GetFilename(), typeName, typTmpl->GetGenericsSize(), typeDf->GenericsSize(), typTmpl->GetLocation(), "TypeError");
            ReportError();
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
        auto typeVar = std::make_shared<TypeVariable>(std::make_shared<ValType>(typeName), false);
        variables->AddItem("self", typeVar);

        m_variables = variables;

        AssumeBlock(body, retType);

        m_variables = currentScope;
    }

    void TypeChecker::HandleRegularDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();
        PValType baseType = nullptr;

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        auto retType = EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "TypeError", this);

        auto fn = HandleFnBasics(fnDecl, exported, retType);

        // Assuming

        auto currentScope = m_variables;
        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);

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

        assert(nameNode->GetType() == NodeType::Identifier && "Name of a normal function should be an identifier");
        std::string name = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

        m_functions->AddItem(name, fn);

        m_variables = variables;

        AssumeBlock(body, retType);

        m_variables = currentScope;
    }

    void TypeChecker::HandleFnSignature(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        PValType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "TypeError", this);

        std::shared_ptr<Node> nameNode = fnDecl->GetName();
        assert(nameNode->GetType() == NodeType::Identifier && "Name should be identifier");
        std::string name = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

        auto fn = HandleFnBasics(fnDecl, exported, retType);

        m_functions->AddItem(name, fn);
    }
}

