
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <error.h>
#include <memory>
#include <filesystem>
#include <dlfcn.h>
#include "../../include/interpreter.h"
#include "include/interpreter/value.h"

namespace fs = std::filesystem;

namespace Runtime
{
    using namespace AST::Nodes;

    std::shared_ptr<Value> Interpreter::EvaluateExternFunctionCall(std::string fnName, std::shared_ptr<Fn> fn, std::vector<std::shared_ptr<Node>>* args)
    {
        Prelude::ErrorManager&errManager = Prelude::ErrorManager::getInstance();

        assert(fn->IsExterned() && "Function should be externed.");
        fs::path fnModule = fn->GetModuleName();
        std::string base = fnModule.filename().string();
        std::string libBase = "lib" + base.replace(base.find(".", 0), 5, ".so");
        fs::path libPath = fnModule.replace_filename(libBase);
        
        void* handle = dlopen(libPath.c_str(), RTLD_NOW);

        if (!handle)
        {
            errManager.RaiseError("failed to open c library at '" + libPath.string() + "': " + dlerror(), "RuntimeError");
            return nullptr;
        }

        void** data = (void**)malloc(sizeof(void*) * fn->GetParamsSize());

        while(fn->HasParams())
        {
            std::shared_ptr<Node> arg = (*args)[fn->GetParamsIndex()];
            auto param = fn->GetNextParam();
            std::shared_ptr<Value> val = Execute(arg);
            switch(val->GetType()) {
                case ValueType::String:
                {
                    auto sVal = std::dynamic_pointer_cast<StringValue>(val);
                    char* str = strdup(sVal->GetValue()->c_str());
                    data[fn->GetParamsIndex() - 1] = (void*)str;
                    break;
                }
                case ValueType::Bool:
                {
                    auto bVal = std::dynamic_pointer_cast<BoolValue>(val);
                    int* b = new int(bVal->GetValue() ? 1 : 0);
                    data[fn->GetParamsIndex() - 1] = (void*)b;
                    break;
                }
                case ValueType::Float:
                {
                    auto fVal = std::dynamic_pointer_cast<FloatValue>(val);
                    float* b = new float(*fVal->GetValue());
                    data[fn->GetParamsIndex() - 1] = (void*)b;
                    break;
                }
                case ValueType::Double:
                {
                    auto dVal = std::dynamic_pointer_cast<DoubleValue>(val);
                    double* b = new double(*dVal->GetValue());
                    data[fn->GetParamsIndex() - 1] = (void*)b;
                    break;
                }
                case ValueType::Integer:
                {
                    auto dVal = std::dynamic_pointer_cast<IntegerValue>(val);
                    int* b = new int(*dVal->GetValue());
                    data[fn->GetParamsIndex() - 1] = (void*)b;
                    break;
                }
                // TODO: Implement list
                default:
                    errManager.RaiseError("Value type '" + HumanValueType(val->GetType())+ "' is not supported in extern functions.", "RuntimeError");
            }
        }

        using LibFn = void*(*)(void**data, unsigned int argc, void* ret);
        LibFn plugfn = (LibFn)dlsym(handle, fnName.c_str());
        if (!plugfn)
        {
            errManager.RaiseError("failed to find function definition at '" + libPath.string() + "': " + dlerror(), "RuntimeError");
            for (size_t i = 0; i < fn->GetParamsSize(); i++)
            {
                free(data[i]);
            }
            free(data);
            dlclose(handle);
            return nullptr;
        }

        void* ret = plugfn(data, fn->GetParamsSize(), ret);

        for (size_t i = 0; i < fn->GetParamsSize(); i++)
        {
            free(data[i]);
        }

        free(data);

        dlclose(handle);

        if (ret) {
            switch(fn->GetReturnType())
            {
                case ValueType::String:
                {
                    char* str = (char*)ret;
                    auto val = std::make_shared<StringValue>(std::string(str));
                    free(str);
                    return val;
                }
                case ValueType::Integer:
                {
                    int* v = (int*)ret;
                    auto val = std::make_shared<IntegerValue>(*v);
                    free(v);
                    return val;
                }
                case ValueType::Float:
                {
                    float* v = (float*)ret;
                    auto val = std::make_shared<FloatValue>(*v);
                    free(v);
                    return val;
                }
                case ValueType::Double:
                {
                    double* v = (double*)ret;
                    auto val = std::make_shared<DoubleValue>(*v);
                    free(v);
                    return val;
                }
                case ValueType::Bool:
                {
                    int* v = (int*)ret;
                    auto val = std::make_shared<BoolValue>(*v == 1);
                    free(v);
                    return val;
                }
                // TODO: Implement list
                default:
                    break;
            }
        }

        return std::make_shared<NullValue>();
    }

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
        fn->ResetIterator();

        if (fn->IsExterned()) {
            return EvaluateExternFunctionCall(fnName, fn, args);
        } else {
            auto currentModule = GetFilename();
            SetFilename(fn->GetModuleName());
            m_variables = variables;

            std::shared_ptr<Value> value = Execute(fn->GetBody());

            m_variables = currentScope;
            SetFilename(currentModule);

            if (value->GetType() != fn->GetReturnType())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.ReturnMismatchType(GetFilename(), fnName, value->GetType(), fn->GetReturnType(), fnCall->GetLocation());
                return nullptr;
            }

            return value;
        }
    }
}

