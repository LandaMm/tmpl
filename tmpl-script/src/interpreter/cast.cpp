
#include "include/interpreter.h"
#include "include/typechecker.h"

namespace Runtime
{
    std::shared_ptr<Value> Interpreter::EvaluateTypeCasting(std::shared_ptr<CastNode> cast)
    {
        auto target = Execute(cast->GetExpr());
        auto targetType = TypeChecker::EvaluateType(GetFilename(), cast->GetTypeNode());

        TypeChecker::CastType(GetFilename(), target->GetType(), targetType, cast->GetTypeNode()->GetLocation(), m_type_definitions, "TypeError");

        target->SetType(targetType);

        return target;
    }
}

