
#include "include/ir/phi.h"
#include "include/ir.h"

namespace IR
{
    std::string PhiInst::Format() const
    {
        return IRTemp::Format() + " = " + FormatDataType(GetDataType()) + " phi(" + GetLeft()->Format() + ", " + GetRight()->Format() + ")";
    }
}

