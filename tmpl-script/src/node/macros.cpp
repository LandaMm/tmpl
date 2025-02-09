
#include "../../include/node/macros.h"

namespace AST
{
    namespace Nodes
    {
        std::string RequireMacro::Format() const
        {
            return "Require(" + m_module + ")";
        }

        std::string ExternMacro::Format() const
        {
            return "Extern(" + m_target->Format() + ")";
        }
    }
}

