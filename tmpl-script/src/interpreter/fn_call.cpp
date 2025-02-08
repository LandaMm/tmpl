
#include <cassert>
#include <memory>
#include "../../include/interpreter.h"

namespace Runtime
{
    using namespace AST::Nodes;

        std::shared_ptr<Value> Interpreter::EvaluateFunctionCall(std::shared_ptr<FunctionCall> fnCall)
        {
            // TODO: support for object member calls
            assert(fnCall->GetCallee()->GetType() == NodeType::Identifier
                    && "Unimplemented fn call callee node type");

            std::shared_ptr<IdentifierNode> callee =
                std::dynamic_pointer_cast<IdentifierNode>(fnCall->GetCallee());

            std::string fnName = callee->GetName();

            if (!m_functions->HasItem(fnName))
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.UndeclaredFunction(GetFilename(), callee, "RuntimeError");
                return nullptr;
            }

            std::shared_ptr<Fn> fn = m_functions->LookUp(fnName);

            if (GetFilename() != fn->GetModuleName() && !fn->IsExported())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.PrivateFunctionError(GetFilename(), fnName, fn->GetModuleName(), callee->GetLocation(), fn->GetLocation(), "RuntimeError");
                return nullptr;
            }

            std::shared_ptr<Environment<Variable>> currentScope = m_variables;
            std::shared_ptr<Environment<Variable>> variables = std::make_shared<Environment<Variable>>(m_variables);

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

            while (fn->HasParams())
            {
                std::shared_ptr<Node> arg = (*args)[fn->GetParamsIndex()];
                std::shared_ptr<FnParam> param = fn->GetNextParam();
                std::shared_ptr<Value> val = Execute(arg);
                if (val->GetType() != param->GetType())
                {
                    Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                    errorManager.ArgMismatchType(
                        GetFilename(),
                        param->GetName(),
                        val->GetType(),
                        param->GetType(),
                        arg->GetLocation(),
                        "RuntimeError"
                    );
                    return nullptr;
                }
                std::shared_ptr<Variable> var =
                    std::make_shared<Variable>(val->GetType(), val, false);
                variables->AddItem(param->GetName(), var);
            }

            m_variables = variables;

            std::shared_ptr<Value> value = Execute(fn->GetBody());

            m_variables = currentScope;

            if (value->GetType() != fn->GetReturnType())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.ReturnMismatchType(GetFilename(), fnName, value->GetType(), fn->GetReturnType(), fnCall->GetLocation());
                return nullptr;
            }

            return value;
        }
}

