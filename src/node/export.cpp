
#include "../../include/node/export.h"

namespace AST
{
    namespace Nodes
    {
        std::string ExportStatement::Format() const
        {
            return "Export(" + m_target->Format() + ")";
        }
    }
}

