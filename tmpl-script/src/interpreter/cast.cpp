
#include "include/interpreter.h"
#include "include/typechecker.h"

namespace Runtime
{
    std::shared_ptr<Value> Interpreter::EvaluateTypeCasting(std::shared_ptr<CastNode> cast)
    {
        auto target = Execute(cast->GetExpr());
        auto targetType = TypeChecker::EvaluateType(GetFilename(), cast->GetTypeNode());

        auto clonedValue = target->Clone();

        return CastValue(clonedValue, targetType, cast->GetTypeNode()->GetLocation());
    }
}

