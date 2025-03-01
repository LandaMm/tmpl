
#include "include/interpreter.h"
#include "include/interpreter/value.h"
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    std::shared_ptr<Value> Interpreter::EvaluateInstance(std::shared_ptr<InstanceNode> instance)
    {
        PValType targetType = TypeChecker::EvaluateType(GetFilename(), instance->GetTarget(), m_type_definitions, "RuntimeError", nullptr);

        if (!m_type_definitions->HasItem(targetType->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), targetType, instance->GetTarget()->GetLocation(), "RuntimeError");
            return nullptr;
        }

        auto typeDf = m_type_definitions->LookUp(targetType->GetName());
        assert(typeDf != nullptr && "Type definition should not be not found at this point.");

        if (GetFilename() != typeDf->GetModuleName() && !typeDf->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateTypeError(GetFilename(), typeDf->GetTypeName(), typeDf->GetModuleName(), instance->GetLocation(), typeDf->GetLocation(), "RuntimeError");
            return std::make_shared<VoidValue>();
        }

        if (!typeDf->HasConstructor())
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeConstructorDoesNotExist(GetFilename(), targetType, instance->GetTarget()->GetLocation(), "RuntimeError");
            return nullptr;
        }

        auto typDfs = std::make_shared<TypeDfs>(m_type_definitions);
        m_type_definitions = typDfs;

        auto gIt = Common::Iterator(typeDf->GenericsSize());
        while (gIt.HasItems())
        {
            auto gen = typeDf->GetGeneric(gIt.GetPosition());
            auto genType = TypeChecker::EvaluateType(GetFilename(), instance->GetTarget()->GetGeneric(gIt.GetPosition()), m_type_definitions, "RuntimeError", nullptr);
            // TODO: check for base and default type
            auto typDf = std::make_shared<TypeDf>(gen->GetName(), genType, GetFilename(), false, instance->GetTarget()->GetGeneric(gIt.GetPosition())->GetLocation());
            typDf->SetTransparent(true);
            typDfs->AddItem(gen->GetName(), typDf);
            gIt.Next();
        }

        auto fn = typeDf->GetConstructor();
        assert(fn != nullptr && "Constructor fn should not be nullptr here.");

        std::shared_ptr<Environment<Variable>> currentScope = m_variables;
        std::shared_ptr<Environment<Variable>> variables = std::make_shared<Environment<Variable>>(m_variables);

        auto fnCall = instance->GetFunctionCall();
        auto fnName = targetType->GetName() + "::constructor";

        if (GetFilename() != fn->GetModuleName() && !fn->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateFunctionError(GetFilename(), fnName, fn->GetModuleName(), fnCall->GetLocation(), fn->GetLocation(), "RuntimeError");
            return nullptr;
        }

        auto args = fnCall->GetArgs();

        if (fn->GetParamsSize() != args->size())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.ArgsParamsExhausted(
                    GetFilename(),
                    fnName,
                    args->size(),
                    fn->GetParamsSize(),
                    fnCall->GetLocation(), "RuntimeError");
            return nullptr;
        }

        auto it = std::make_shared<Common::Iterator>(fn->GetParamsSize());
        while (it->HasItems())
        {
            auto param = fn->GetItem(it->GetPosition());
            std::shared_ptr<Node> arg = (*args)[it->GetPosition()];
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

        auto currentModule = GetFilename();
        SetFilename(fn->GetModuleName());
        m_variables = variables;

        std::shared_ptr<Value> value = Execute(fn->GetBody());

        m_variables = currentScope;
        SetFilename(currentModule);

        if (!value->GetType()->Compare(*fn->GetReturnType()))
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.ReturnMismatchType(GetFilename(), fnName, value->GetType(), fn->GetReturnType(), fnCall->GetLocation());
            return nullptr;
        }

        value->SetType(targetType);

        assert(m_type_definitions->GetParent() != nullptr && "Parent gone.");
        assert(m_type_definitions->GetParent() != m_type_definitions && "Typ df parent clone");
        m_type_definitions = m_type_definitions->GetParent();

        return value;
    }
}

