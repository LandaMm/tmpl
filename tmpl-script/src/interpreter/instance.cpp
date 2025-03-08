
#include "include/interpreter.h"
#include "include/interpreter/value.h"
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    std::shared_ptr<Value> Interpreter::EvaluateInstance(std::shared_ptr<InstanceNode> instance)
    {
        if (!m_type_definitions->HasItem(instance->GetTarget()->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), instance->GetTarget()->GetName(), instance->GetTarget()->GetLocation(), "RuntimeError");
            return nullptr;
        }

        auto typeDf = m_type_definitions->LookUp(instance->GetTarget()->GetName());
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
            errManager.TypeConstructorDoesNotExist(GetFilename(), std::make_shared<ValType>(instance->GetTarget()->GetName()), instance->GetTarget()->GetLocation(), "RuntimeError");
            return nullptr;
        }

        auto genHandler = GenHandler(GetFilename(), m_type_definitions, "RuntimeError", nullptr);

        auto typDfs = std::make_shared<TypeDfs>(m_type_definitions);
        m_type_definitions = typDfs;

        auto gIt = Common::Iterator(typeDf->GenericsSize());
        while (gIt.HasItems())
        {
            auto gen = typeDf->GetGeneric(gIt.GetPosition());
            auto genType = TypeChecker::EvaluateType(GetFilename(), instance->GetFunctionCall()->GetGeneric(gIt.GetPosition()), m_type_definitions, "RuntimeError", nullptr);

            genHandler.DefineGeneric(gen, genType);

            gIt.Next();
        }

        auto fn = typeDf->GetConstructor();
        assert(fn != nullptr && "Constructor fn should not be nullptr here.");

        std::shared_ptr<Environment<Variable>> currentScope = m_variables;
        std::shared_ptr<Environment<Variable>> variables = std::make_shared<Environment<Variable>>(m_variables);

        auto fnCall = instance->GetFunctionCall();
        auto fnName = instance->GetTarget()->GetName() + "::constructor";

        if (GetFilename() != fn->GetModuleName() && !fn->IsExported())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.PrivateFunctionError(GetFilename(), fnName, fn->GetModuleName(), fnCall->GetLocation(), fn->GetLocation(), "RuntimeError");
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

        auto currentModule = GetFilename();
        SetFilename(fn->GetModuleName());
        m_variables = variables;

        std::shared_ptr<Value> value = Execute(fn->GetBody());

        m_variables = currentScope;
        SetFilename(currentModule);

        auto fnRetType = TypeChecker::NormalizeType(GetFilename(), fn->GetReturnType(), fn->GetLocation(), m_type_definitions, "RuntimeError", nullptr);

        if (!value->GetType()->Compare(*fnRetType))
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.ReturnMismatchType(GetFilename(), fnName, value->GetType(), fn->GetReturnType(), fnCall->GetLocation());
            return nullptr;
        }

        m_type_definitions = genHandler.Unload();

        auto valType = std::make_shared<ValType>(instance->GetTarget()->GetName());

        auto genIt = Common::Iterator(fnCall->GetGenericsSize());
        while (genIt.HasItems())
        {
            auto genNode = fnCall->GetGeneric(genIt.GetPosition());
            PValType genType = TypeChecker::EvaluateType(GetFilename(), genNode, m_type_definitions, "RuntimeError", nullptr);
            valType->AddGeneric(genType);
            genIt.Next();
        }

        value->SetType(valType);

        return value;
    }
}

