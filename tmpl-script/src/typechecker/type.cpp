

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

    PValType TypeChecker::CastType(std::string filename, PValType from, PValType to, TypeChecker::PTypeDfs typeDfs)
    {
        // TODO:
        // 0. check if "from" and "to" types are equal
        // 1. check if "from" type exists in typeDfs
        // 2. check if "to" type exists in typeDfs
        // 3. check if "to" type's name is "from" type's basename
        // 4. otherwise check if "from" type contains cast to "to" type in typeDf
    }
}

