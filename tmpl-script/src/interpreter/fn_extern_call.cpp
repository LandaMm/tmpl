
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <filesystem>
#include <string.h>
#include "../../include/interpreter.h"
#include "../../include/interpreter/value.h"

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

#ifdef _WIN32
        std::string libBase = base.replace(base.find(".", 0), 5, ".dll");
#else
        std::string libBase = "lib" + base.replace(base.find(".", 0), 5, ".so");
#endif

        fs::path libPath = fnModule.replace_filename(libBase);
        
#ifdef _WIN32
        HMODULE handle = LoadLibrary(libPath.string().c_str());
#else
        void* handle = dlopen(libPath.c_str(), RTLD_NOW);
#endif

        if (!handle)
        {
#ifdef _WIN32
            errManager.RaiseError("failed to open c library at '" + libPath.string() + "': " + std::to_string(GetLastError()), "RuntimeError");
#else
            errManager.RaiseError("failed to open c library at '" + libPath.string() + "': " + dlerror(), "RuntimeError");
#endif
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
#ifdef _WIN32
                    char* str = _strdup(sVal->GetValue()->c_str());
#else
                    char* str = strdup(sVal->GetValue()->c_str());
#endif
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

        using LibFn = void*(*)(void**data, unsigned int argc);

#ifdef _WIN32
        LibFn plugfn = (LibFn)GetProcAddress(handle, fnName.c_str());
#else
        LibFn plugfn = (LibFn)dlsym(handle, fnName.c_str());
#endif

        if (!plugfn)
        {
#ifdef _WIN32
            errManager.RaiseError("Failed to find function at '" + libPath.string() + "': " + std::to_string(GetLastError()), "RuntimeError");
#else
            errManager.RaiseError("Failed to find function at '" + libPath.string() + "': " + dlerror(), "RuntimeError");
#endif

            for (size_t i = 0; i < fn->GetParamsSize(); i++)
            {
                free(data[i]);
            }
            free(data);

#ifdef _WIN32
            FreeLibrary(handle);
#else
            dlclose(handle);
#endif
            return nullptr; 
        }

        void* ret = plugfn(data, fn->GetParamsSize());

        for (size_t i = 0; i < fn->GetParamsSize(); i++)
        {
            free(data[i]);
        }
        free(data);

#ifdef _WIN32
        FreeLibrary(handle);
#else
        dlclose(handle);
#endif

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
}

