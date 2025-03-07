
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

    void Helper::DefineBuiltInListType(PTypeDfEnv env)
    {
        std::string bName = "#BUILTIN_LIST";
        std::string tName = "list";

        DefineBuiltInType(bName, tName, env);

        auto bTypDf = env->LookUp(bName);
        auto tTypDf = env->LookUp(tName);
        assert(bTypDf != nullptr && tTypDf != nullptr && "List type definitions should exist now.");

        auto typGen = std::make_shared<Runtime::TypeDfGeneric>("T", AST::Location(-1, -1));
        
        bTypDf->AddGeneric(typGen);
        tTypDf->AddGeneric(typGen);
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

        auto mixedTypDf = std::make_shared<Runtime::TypeDf>("#BUILTIN_MIXED", nullptr, "#GLOBAL", true, AST::Location(-1, -1));
        typeDefinitions->AddItem("#BUILTIN_MIXED", mixedTypDf);

        DefineBuiltInListType(typeDefinitions);

        return typeDefinitions;
    }
}

