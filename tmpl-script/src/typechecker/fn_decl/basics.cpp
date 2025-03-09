
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    std::shared_ptr<Fn> TypeChecker::HandleFnBasics(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported)
    {
        FnModifier modifier;
        switch(fnDecl->GetModifier())
        {
            case AST::Nodes::FunctionModifier::Cast:
                modifier = FnModifier::Cast;
                break;
            case AST::Nodes::FunctionModifier::Construct:
                modifier = FnModifier::Construct;
                break;
            case AST::Nodes::FunctionModifier::None:
                if (fnDecl->GetName()->GetType() == NodeType::ObjectMember)
                    modifier = FnModifier::Type;
                else
                    modifier = FnModifier::Regular;
                break;
            default:
                assert(false && "Not all fn modifiers are being handled");
        }

        std::shared_ptr<Fn> fn = std::make_shared<Fn>(fnDecl->GetBody(), nullptr, modifier, GetFilename(), exported, false, fnDecl->GetLocation());

        // Generics
        auto gIt = Common::Iterator(fnDecl->GetGenericsSize());
        while (gIt.HasItems())
        {
            std::shared_ptr<TemplateGeneric> tGen = fnDecl->GetGeneric(gIt.GetPosition());
            
            auto gen = std::make_shared<FnGeneric>(tGen->GetName(), tGen->GetLocation());
            fn->AddGeneric(gen);

            gIt.Next();
        }

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "TypeError", this);
        genHandler.DefineGenerics(fn->GetGenIterator(), false);

        // Params
        auto it = Common::Iterator(fnDecl->GetParamsSize());
        while (it.HasItems())
        {
            auto param = fnDecl->GetParam(it.GetPosition());
            PValType paramType = EvaluateType(GetFilename(), param->GetType(), m_type_definitions, "TypeError", this);
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
            it.Next();
        }

        m_type_definitions = genHandler.Unload();

        return fn;
    }
}

