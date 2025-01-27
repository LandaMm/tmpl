
#include "../../include/node/procedure.h"

namespace AST
{
    namespace Nodes
    {
        std::string ProcedureDeclaration::Format() const
        {
            return "ProcedureDeclaration(" + m_name + ")";
        }
    }
}
