

#include "../../include/typechecker.h"
#include "include/interpreter/environment.h"
#include "include/interpreter/value.h"
#include "include/iterator.h"
#include "include/node/function.h"
#include "include/node/object_member.h"
#include <memory>

namespace Runtime
{
    using namespace AST::Nodes;

    PValType TypeChecker::ResolveFn(std::shared_ptr<Fn> fn, std::string fnName, std::shared_ptr<FunctionCall> fnCall)
    {
        if (GetFilename() != fn->GetModuleName() && !fn->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateFunctionError(GetFilename(), fnName, fn->GetModuleName(), fnCall->GetLocation(), fn->GetLocation(), "TypeError");
            ReportError();
            return std::make_shared<ValType>("void");
        }

        if (fn->GetParamsSize() != fnCall->GetArgumentsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.ArgsParamsExhausted(
                    GetFilename(),
                    fnName,
                    fnCall->GetArgumentsSize(),
                    fn->GetParamsSize(),
                    fnCall->GetLocation(), "TypeError");
            ReportError();
            return fn->GetReturnType();
        }

        if (fn->GetModifier() != FnModifier::Type && fn->GetGenericsSize() != fnCall->GetGenericsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.TypeGenericsExhausted(GetFilename(), fnName, fnCall->GetGenericsSize(), fn->GetGenericsSize(), fnCall->GetLocation(), "TypeError");
            ReportError();
            return std::make_shared<ValType>("void");
        }
        else if (fn->GetModifier() == FnModifier::Type && fnCall->GetGenericsSize() > 0)
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.TypeGenericsExhausted(GetFilename(), fnName, fnCall->GetGenericsSize(), 0, fnCall->GetLocation(), "TypeError");
            ReportError();
            return std::make_shared<ValType>("void");
        }

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "TypeError", this);

        auto gIt = Common::Iterator(fn->GetGenericsSize());
        PValType targetTyp = nullptr;

        if (fn->GetModifier() == FnModifier::Type)
        {
            assert(fnCall->GetCallee()->GetType() == NodeType::ObjectMember && "Type fn callee should be an object");
            auto obj = std::dynamic_pointer_cast<ObjectMember>(fnCall->GetCallee());
            targetTyp = DiagnoseNode(obj->GetObject());
            if (targetTyp->GetGenericsSize() != fn->GetGenericsSize())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.TypeGenericsExhausted(GetFilename(), fnName, targetTyp->GetGenericsSize(), fn->GetGenericsSize(), obj->GetObject()->GetLocation(), "TypeError");
                return fn->GetReturnType();
            }
        }

        while (gIt.HasItems())
        {
            auto gen = fn->GetGeneric(gIt.GetPosition());

            if (fn->GetModifier() == FnModifier::Type)
            {
                assert(targetTyp != nullptr && "Target type should have been set in case of type functions");
                auto genType = targetTyp->GetGeneric(gIt.GetPosition());

                genHandler.DefineGeneric(gen, genType);
            }
            else
            {
                auto genTypeNode = fnCall->GetGeneric(gIt.GetPosition());
                auto genType = EvaluateType(GetFilename(), genTypeNode, m_type_definitions, "TypeError", this);

                genHandler.DefineGeneric(gen, genType);
            }

            gIt.Next();
        }

        auto it = std::make_shared<Common::Iterator>(fn->GetParamsSize());
        while (it->HasItems())
        {
            auto param = fn->GetParam(it->GetPosition());
            std::shared_ptr<Node> arg = fnCall->GetArgument(it->GetPosition());
            PValType valType = DiagnoseNode(arg);
            PValType paramType = NormalizeType(GetFilename(), param->GetType(), arg->GetLocation(), m_type_definitions, "TypeError", this);
            if (!valType->Compare(*paramType))
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.ArgMismatchType(
                        GetFilename(),
                        param->GetName(),
                        valType,
                        paramType,
                        arg->GetLocation(),
                        "TypeError"
                        );
                ReportError();
                break; // will get to return of the fn's return type (+4 lines)
            }

            it->Next();
        }

        auto retType = NormalizeType(GetFilename(), fn->GetReturnType(), fnCall->GetLocation(), m_type_definitions, "TypeError", this);

        m_type_definitions = genHandler.Unload();

        return retType;
    }

    bool TypeChecker::ResolveTypeFn(std::shared_ptr<ObjectMember> obj, std::shared_ptr<Fn>& fn, std::string& fnName)
    {
        PValType targetType = DiagnoseNode(obj->GetObject());
        auto typeName = targetType->GetName();

        if (!m_type_definitions->HasItem(typeName))
        {
            Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), typeName, obj->GetObject()->GetLocation(), "TypeError");
            ReportError();
            return false;
        }

        auto typeDf = m_type_definitions->LookUp(typeName);
        assert(typeDf != nullptr && "Type df shouldn't be null at this point.");

        if (GetFilename() != typeDf->GetModuleName() && !typeDf->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateTypeError(GetFilename(), typeDf->GetTypeName(), typeDf->GetModuleName(), obj->GetObject()->GetLocation(), typeDf->GetLocation(), "TypeError");
            ReportError();
            return false;
        }

        auto typeEnv = typeDf->GetTypFnsEnv();
        assert(typeEnv != nullptr && "Type env should have been created.");

        std::shared_ptr<Node> fnNameNode = obj->GetMember();
        fnName = std::dynamic_pointer_cast<IdentifierNode>(fnNameNode)->GetName();
        // TODO: think if "Contains" is better in that case
        if (!typeEnv->HasItem(fnName))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.UndeclaredFunction(GetFilename(), obj, targetType, "TypeError");
            ReportError();
            return false;
        }

        fn = typeEnv->LookUp(fnName);
        return true;
    }

    PValType TypeChecker::DiagnoseFnCall(std::shared_ptr<FunctionCall> fnCall)
    {
        // TODO: support for object member calls
        std::shared_ptr<Node> callee = fnCall->GetCallee();

        std::shared_ptr<Fn> fn;
        std::string fnName;

        switch(callee->GetType())
        {
            case AST::NodeType::ObjectMember:
            {
                if (!ResolveTypeFn(std::dynamic_pointer_cast<ObjectMember>(callee), fn, fnName))
                {
                    // encourented an error
                    return std::make_shared<ValType>("void");
                }
                break;
            }
            case AST::NodeType::Identifier:
            {
                auto idCallee = std::dynamic_pointer_cast<IdentifierNode>(callee);
                fnName = idCallee->GetName();

                if (!m_functions->HasItem(fnName))
                {
                    Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                    errorManager.UndeclaredFunction(GetFilename(), idCallee, "TypeError");
                    ReportError();
                    return std::make_shared<ValType>("void");
                }

                fn = m_functions->LookUp(fnName);
                break;
            }
            default:
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.RaiseError("Invalid node for function call", "TypeError");
                ReportError();
                return std::make_shared<ValType>("void");
            }
        }

        return ResolveFn(fn, fnName, fnCall);
    }
}


