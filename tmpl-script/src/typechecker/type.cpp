

#include "../../include/typechecker.h"
#include "include/interpreter/value.h"
#include <memory>

namespace Runtime
{
    using namespace AST::Nodes;

    PValType TypeChecker::EvaluateType(std::string filename, std::shared_ptr<TypeNode> typeNode)
    {
        return std::make_shared<ValType>(typeNode->GetTypeName()->GetName());
    }
}

