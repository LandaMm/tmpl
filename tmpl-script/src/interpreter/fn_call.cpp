
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
                errorManager.UndeclaredFunction(m_parser->GetFilename(), callee);
                return nullptr;
            }

            std::shared_ptr<Fn> fn = m_functions->LookUp(fnName);

            std::shared_ptr<Environment<Variable>> currentScope = m_variables;
            std::shared_ptr<Environment<Variable>> variables = std::make_shared<Environment<Variable>>(m_variables);

            auto params = fn->GetParams();
            auto args = fnCall->GetArgs();

            if (params.size() != args->size())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.ArgsParamsExhausted(
                        m_parser->GetFilename(),
                        fnName,
                        args->size(),
                        params.size(),
                        fnCall->GetLocation());
                return nullptr;
            }

            for (size_t i = 0; i < params.size(); i++)
            {
                FnParam param = params[i];
                std::shared_ptr<Node> arg = (*args)[i];
                std::shared_ptr<Value> val = Execute(arg);
                if (val->GetType() != param.GetType())
                {
                    Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                    errorManager.ArgMismatchType(
                        m_parser->GetFilename(),
                        param.GetName(),
                        val->GetType(),
                        param.GetType(),
                        arg->GetLocation()
                    );
                    return nullptr;
                }
                std::shared_ptr<Variable> var =
                    std::make_shared<Variable>(val->GetType(), val, false);
                variables->AddItem(param.GetName(), var);
            }

            m_variables = variables;

            std::shared_ptr<Value> value = Execute(fn->GetBody());

            m_variables = currentScope;

            if (value->GetType() != fn->GetReturnType())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.ReturnMismatchType(m_parser->GetFilename(), fnName, value->GetType(), fn->GetReturnType(), fnCall->GetLocation());
                return nullptr;
            }

            return value;
        }
}

