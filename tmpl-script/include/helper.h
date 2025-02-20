
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
    };
}

#endif // HELPER_H

