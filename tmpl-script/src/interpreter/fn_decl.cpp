

#include <memory>
#include "../../include/interpreter.h"
#include "include/node/function.h"

namespace Runtime
{
    using namespace AST::Nodes;

    void Interpreter::EvaluateFunctionDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        switch(fnDecl->GetModifier())
        {
            case AST::Nodes::FunctionModifier::Construct:
                return EvaluateConstructDeclaration(fnDecl, exported, externed);
            case AST::Nodes::FunctionModifier::Cast:
                return EvaluateCastDeclaration(fnDecl, exported, externed);
            case AST::Nodes::FunctionModifier::None:
                if (fnDecl->GetName()->GetType() == NodeType::Identifier)
                    return EvaluateRegularFnDeclaration(fnDecl, exported, externed);
                else
                    return EvaluateTypeFnDeclaration(fnDecl, exported, externed);
        }

        assert(false && "Should handle every fn modifier");
    }

}

