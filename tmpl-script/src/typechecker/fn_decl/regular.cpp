
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    void TypeChecker::HandleRegularDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();
        PValType baseType = nullptr;

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        auto fn = HandleFnBasics(fnDecl, exported);

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "TypeError", this);
        genHandler.DefineGenerics(fn->GetGenIterator(), false);

        auto retType = EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "TypeError", this);

        fn->SetReturnType(retType);

        m_type_definitions = genHandler.Unload();
        
        // Assuming
        
        auto aGenHandler = GenHandler(GetFilename(), m_type_definitions, "TypeError", this);
        aGenHandler.DefineGenerics(fn->GetGenIterator(), true);

        auto currentScope = m_variables;
        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);

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

        assert(nameNode->GetType() == NodeType::Identifier && "Name of a normal function should be an identifier");
        std::string name = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

        m_functions->AddItem(name, fn);

        m_variables = variables;

        AssumeBlock(body, retType);

        m_variables = currentScope;

        m_type_definitions = aGenHandler.Unload();
    }
}

