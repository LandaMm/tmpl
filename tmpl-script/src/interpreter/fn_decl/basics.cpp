
#include "include/interpreter.h"
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    std::shared_ptr<Fn> Interpreter::EvaluateFnDeclarationBasics(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        std::shared_ptr<Fn> fn = std::make_shared<Fn>(fnDecl->GetBody(), nullptr, GetFilename(), exported, externed, fnDecl->GetLocation());

        // Generics
        auto gIt = Common::Iterator(fnDecl->GetGenericsSize());
        while (gIt.HasItems())
        {
            std::shared_ptr<TemplateGeneric> tGen = fnDecl->GetGeneric(gIt.GetPosition());
            
            auto gen = std::make_shared<FnGeneric>(tGen->GetName(), tGen->GetLocation());
            fn->AddGeneric(gen);

            gIt.Next();
        }

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "RuntimeError", nullptr);
        genHandler.DefineGenerics(fn->GetGenIterator(), false);
        
        // Params
        auto it = Common::Iterator(fnDecl->GetParamsSize());
        while (it.HasItems())
        {
            auto param = fnDecl->GetParam(it.GetPosition());
            PValType paramType = TypeChecker::EvaluateType(GetFilename(), param->GetType(), m_type_definitions, "RuntimeError", nullptr);
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
            it.Next();
        }

        m_type_definitions = genHandler.Unload();

        return fn;
    }
}

