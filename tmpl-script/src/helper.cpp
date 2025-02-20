
#include "include/helper.h"
#include "include/interpreter/value.h"
#include "include/typechecker/typedf.h"

namespace Helper
{
    Helper::PTypeDfEnv Helper::GetTypeDefinitions()
    {
        auto typeDefinitions = std::make_shared<Helper::TypeDfEnv>();

        std::string bIntName = "#BUILTIN_INT";

        auto bInt = std::make_shared<Runtime::TypeDf>(bIntName, nullptr);
        auto bIntType = std::make_shared<Runtime::ValType>(bIntName);
        typeDefinitions->AddItem(bIntName, bInt);

        auto Int = std::make_shared<Runtime::TypeDf>("int", bIntType);
        typeDefinitions->AddItem("int", Int);

        return typeDefinitions;
    }
}

