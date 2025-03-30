
#include "include/ir/block.h"
#include "include/iterator.h"

namespace IR
{
    std::string BlockInst::Format() const 
    {
        std::string formatted = "\n" + GetLabel()->Format() + ":";
        for (auto it = Common::Iterator(GetSize()); it.HasItems(); it.Next())
        {
            auto inst = GetInst(it.GetPosition());
            formatted += "\n";
            formatted += inst->Format();
        }
        return formatted;
    }
}

