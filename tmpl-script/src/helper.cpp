
#include "include/helper.h"
#include "include/interpreter/value.h"
#include "include/location.h"
#include "include/typechecker/typedf.h"

namespace Helper
{
    void Helper::DefineBuiltInType(std::string builtinName, std::string name, PTypeDfEnv env)
    {
        auto bTypeDf = std::make_shared<Runtime::TypeDf>(builtinName, nullptr, "#GLOBAL", true, AST::Location(-1, -1));
        auto bType = std::make_shared<Runtime::ValType>(builtinName);
        env->AddItem(builtinName, bTypeDf);

        auto Type = std::make_shared<Runtime::TypeDf>(name, bType, "#GLOBAL", true, AST::Location(-1, -1));
        env->AddItem(name, Type);
    }

    Helper::PTypeDfEnv Helper::GetTypeDefinitions()
    {
        auto typeDefinitions = std::make_shared<Helper::TypeDfEnv>();

        DefineBuiltInType("#BUILTIN_INT", "int", typeDefinitions);
        DefineBuiltInType("#BUILTIN_FLOAT", "float", typeDefinitions);
        DefineBuiltInType("#BUILTIN_DOUBLE", "double", typeDefinitions);
        DefineBuiltInType("#BUILTIN_STRING", "string", typeDefinitions);
        DefineBuiltInType("#BUILTIN_BOOL", "bool", typeDefinitions);
        DefineBuiltInType("#BUILTIN_VOID", "void", typeDefinitions);
        // TODO: list type

        return typeDefinitions;
    }
}

