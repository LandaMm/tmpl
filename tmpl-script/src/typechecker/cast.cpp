#include "include/typechecker.h"

namespace Runtime
{
    PValType TypeChecker::DiagnoseTypeCasting(std::shared_ptr<CastNode> cast)
    {
        auto target = DiagnoseNode(cast->GetExpr());
        auto targetType = EvaluateType(GetFilename(), cast->GetTypeNode(), m_type_definitions, "TypeError", this);

        return CastType(GetFilename(), target, targetType, cast->GetTypeNode()->GetLocation(), m_type_definitions, "TypeError");
    }
}

