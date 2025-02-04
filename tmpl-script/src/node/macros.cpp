
#include "../../include/node/macros.h"

namespace AST
{
    namespace Nodes
    {
        std::string RequireMacro::Format() const
        {
            return "Require(" + m_module + ")";
        }
    }
}

