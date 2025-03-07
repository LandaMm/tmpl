
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <error.h>
#include <memory>
#include "../../include/interpreter.h"
#include "include/interpreter/value.h"
#include "include/iterator.h"
#include "include/node.h"
#include "include/node/identifier.h"
#include "include/node/object_member.h"
#include "include/typechecker.h"

namespace Runtime
{
    using namespace AST::Nodes;

    std::shared_ptr<Value> Interpreter::EvaluateFunctionCall(std::shared_ptr<FunctionCall> fnCall)
    {
        std::shared_ptr<Node> callee = fnCall->GetCallee();

        std::shared_ptr<Fn> fn;
        std::string fnName;

        std::shared_ptr<Environment<Variable>> currentScope = m_variables;
        std::shared_ptr<Environment<Variable>> variables = std::make_shared<Environment<Variable>>(m_variables);

        switch(callee->GetType())
        {
            case AST::NodeType::ObjectMember:
            {
                auto obj = std::dynamic_pointer_cast<ObjectMember>(callee);
                std::shared_ptr<Value> targetVal = Execute(obj->GetObject());
                PValType targetType = targetVal->GetType();

                auto typDf = m_type_definitions->LookUp(targetType->GetName());
                assert(typDf != nullptr && "Found typ df should not be nullptr");

                std::shared_ptr<Environment<Fn>> typeEnv = typDf->GetTypFnsEnv();
                assert(typeEnv != nullptr && "Type env should have been created.");

                std::shared_ptr<Node> fnNameNode = obj->GetMember();
                fnName = std::dynamic_pointer_cast<IdentifierNode>(fnNameNode)->GetName();
                // TODO: think if "Contains" is better in that case
                if (!typeEnv->HasItem(fnName))
                {
                    Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
                    errManager.UndeclaredFunction(GetFilename(), obj, targetType, "RuntimeError");
                    return nullptr;
                }

                auto selfVar = std::make_shared<Variable>(targetType, targetVal, false);
                variables->AddItem("self", selfVar);

                fn = typeEnv->LookUp(fnName);
                break;
            }
            case AST::NodeType::Identifier:
            {
                auto idCallee = std::dynamic_pointer_cast<IdentifierNode>(callee);
                fnName = idCallee->GetName();

                if (!m_functions->HasItem(fnName))
                {
                    Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                    errorManager.UndeclaredFunction(GetFilename(), idCallee, "RuntimeError");
                    return nullptr;
                }

                fn = m_functions->LookUp(fnName);
                break;
            }
            default:
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.RaiseError("Invalid node for function call", "RuntimeError");
                return nullptr;
            }
        }

        assert(fn != nullptr && "Fn should not be left with nullptr");
        assert(!fnName.empty() && "Function name should not have been left empty");

        if (GetFilename() != fn->GetModuleName() && !fn->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateFunctionError(GetFilename(), fnName, fn->GetModuleName(), callee->GetLocation(), fn->GetLocation(), "RuntimeError");
            return nullptr;
        }

        if (fn->GetParamsSize() != fnCall->GetArgumentsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.ArgsParamsExhausted(
                    GetFilename(),
                    fnName,
                    fnCall->GetArgumentsSize(),
                    fn->GetParamsSize(),
                    fnCall->GetLocation(), "RuntimeError");
            return nullptr;
        }

        if (fn->GetGenericsSize() != fnCall->GetGenericsSize())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.TypeGenericsExhausted(GetFilename(), fnName, fnCall->GetGenericsSize(), fn->GetGenericsSize(), fnCall->GetLocation(), "RuntimeError");
            return nullptr;
        }

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "TypeError", nullptr);

        auto gIt = Common::Iterator(fn->GetGenericsSize());
        while (gIt.HasItems())
        {
            auto gen = fn->GetGeneric(gIt.GetPosition());
            auto genTypeNode = fnCall->GetGeneric(gIt.GetPosition());

            auto genType = TypeChecker::EvaluateType(GetFilename(), genTypeNode, m_type_definitions, "RuntimeError", nullptr);

            genHandler.DefineGeneric(gen, genType);

            gIt.Next();
        }

        auto it = std::make_shared<Common::Iterator>(fn->GetParamsSize());
        while (it->HasItems())
        {
            auto param = fn->GetParam(it->GetPosition());
            std::shared_ptr<Node> arg = fnCall->GetArgument(it->GetPosition());
            it->Next();
            std::shared_ptr<Value> val = Execute(arg);
            PValType paramType = TypeChecker::NormalizeType(GetFilename(), param->GetType(), arg->GetLocation(), m_type_definitions, "RuntimeError", nullptr);
            if (!val->GetType()->Compare(*paramType))
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.ArgMismatchType(
                        GetFilename(),
                        param->GetName(),
                        val->GetType(),
                        paramType,
                        arg->GetLocation(),
                        "RuntimeError"
                        );
                return nullptr;
            }
            if (!fn->IsExterned())
            {
                std::shared_ptr<Variable> var =
                    std::make_shared<Variable>(val->GetType(), val, false);
                variables->AddItem(param->GetName(), var);
            }
        }

        if (fn->IsExterned()) {
            auto retVal = EvaluateExternFunctionCall(fnName, fn, fnCall);

            m_type_definitions = m_type_definitions->GetParent();
            
            return retVal;
        } else {
            auto currentModule = GetFilename();
            SetFilename(fn->GetModuleName());
            m_variables = variables;

            std::shared_ptr<Value> value = Execute(fn->GetBody());

            m_variables = currentScope;
            SetFilename(currentModule);

            auto retType = TypeChecker::NormalizeType(GetFilename(), fn->GetReturnType(), fnCall->GetLocation(), m_type_definitions, "RuntimeError", nullptr);

            m_type_definitions = genHandler.Unload();

            if (!value->GetType()->Compare(*retType))
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.ReturnMismatchType(GetFilename(), fnName, value->GetType(), fn->GetReturnType(), fnCall->GetLocation());
                return nullptr;
            }

            return value;
        }
    }
}

