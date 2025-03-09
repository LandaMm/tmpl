

#include "../../include/typechecker.h"
#include "../../include/node.h"
#include "../../include/node/identifier.h"
#include "include/interpreter/value.h"
#include "include/node/function.h"
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

    void TypeChecker::HandleFnSignature(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        PValType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "TypeError", this);

        std::shared_ptr<Node> nameNode = fnDecl->GetName();
        assert(nameNode->GetType() == NodeType::Identifier && "Name should be identifier");
        std::string name = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

        auto fn = HandleFnBasics(fnDecl, exported);
        fn->SetReturnType(retType);

        m_functions->AddItem(name, fn);
    }
}

