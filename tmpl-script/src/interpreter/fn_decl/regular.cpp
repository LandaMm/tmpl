
#include "include/interpreter.h"
#include "include/node/function.h"
#include "include/typechecker.h"

namespace Runtime
{
    void Interpreter::EvaluateRegularFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        auto fn = EvaluateFnDeclarationBasics(fnDecl, exported, externed);

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "TypeError", nullptr);
        genHandler.DefineGenerics(fn->GetGenIterator(), false);

        auto retType = TypeChecker::EvaluateType(GetFilename(), fnDecl->GetReturnType(), m_type_definitions, "RuntimeError", nullptr);

        fn->SetReturnType(retType);
        m_type_definitions = genHandler.Unload();

        std::shared_ptr<Node> nameNode = fnDecl->GetName();

        std::string name = std::dynamic_pointer_cast<IdentifierNode>(nameNode)->GetName();

        assert(fnDecl->GetModifier() == FunctionModifier::None && "regular function should have no modifier");
        m_functions->AddItem(name, fn);
    }
}

