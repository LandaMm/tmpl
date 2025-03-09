
#ifndef HELPER_H
#define HELPER_H

#include "interpreter/environment.h"
#include "include/typechecker/typedf.h"

namespace Helper
{
    class Helper
    {
    private:
        using TypeDfEnv = Runtime::Environment<Runtime::TypeDf>;
        using PTypeDfEnv = std::shared_ptr<TypeDfEnv>;
    public:
        static PTypeDfEnv GetTypeDefinitions();
        static void DefineBuiltInType(std::string builtinName, std::string name, PTypeDfEnv env);
        static void DefineBuiltInListType(PTypeDfEnv env);
    };
}

#endif // HELPER_H

