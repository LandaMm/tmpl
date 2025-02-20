
#include "include/interpreter.h"

namespace Runtime
{
    std::shared_ptr<Value> Interpreter::CastValue(std::shared_ptr<Value> val, PValType to, Location loc)
    {
        auto typeDfs = m_type_definitions;
        auto from = val->GetType();
        // 0. check if "from" type exists in typeDfs
        if (!typeDfs->HasItem(from->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), from, loc, "RuntimeError");
            return std::make_shared<VoidValue>();
        }
        // 1. check if "to" type exists in typeDfs
        if (!typeDfs->HasItem(to->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(GetFilename(), to, loc, "RuntimeError");
            return std::make_shared<VoidValue>();
        }
        // 2. check if "from" and "to" types are equal
        if (from->Compare(*to))
        {
            val->SetType(to);
            return val;
        }
        // 3. check if "from" type's name is "to" type's basename
        auto fromDf = typeDfs->LookUp(from->GetName());
        assert(fromDf != nullptr && "From type's definition should not be null");
        if (fromDf->GetBaseType()->Compare(*to)) 
        {
            val->SetType(to);
            return val;
        }
        // 4. otherwise check if "from" type contains cast to "to" type in typeDf
        auto casts = fromDf->GetCastsEnv();
        if (casts->HasItem(to->GetName()))
        {
            auto cast = casts->LookUp(to->GetName());
            assert(cast != nullptr && "Found cast should not be null");

            auto fnName = "cast(" + cast->GetOrigin() + " -> " + cast->GetTarget() + ")";
            auto fn = cast->GetCaster();

            std::shared_ptr<Environment<Variable>> currentScope = m_variables;
            std::shared_ptr<Environment<Variable>> variables = std::make_shared<Environment<Variable>>(m_variables);

            auto selfVar = std::make_shared<Variable>(from, val, false);
            variables->AddItem("self", selfVar);

            if (GetFilename() != fn->GetModuleName() && !fn->IsExported())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.PrivateFunctionError(GetFilename(), fnName, fn->GetModuleName(), loc, fn->GetLocation(), "RuntimeError");
                return nullptr;
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
                errorManager.ReturnMismatchType(GetFilename(), fnName, value->GetType(), fn->GetReturnType(), loc);
                return nullptr;
            }

            return value;
        }

        Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
        errManager.TypeCastNotPossible(GetFilename(), from, to, loc, "RuntimeError");
        return std::make_shared<VoidValue>();
    }
}

