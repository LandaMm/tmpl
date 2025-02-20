

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

    PValType TypeChecker::CastType(std::string filename, PValType from, PValType to, Location loc, TypeChecker::PTypeDfs typeDfs, std::string prefix)
    {
        // 0. check if "from" type exists in typeDfs
        if (!typeDfs->HasItem(from->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(filename, from, loc, prefix);
            return std::make_shared<ValType>("void");
        }
        // 1. check if "to" type exists in typeDfs
        if (!typeDfs->HasItem(to->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(filename, to, loc, prefix);
            return std::make_shared<ValType>("void");
        }
        // 2. check if "from" and "to" types are equal
        if (from->Compare(*to)) return to;
        // 3. check if "from" type's name is "to" type's basename
        auto fromDf = typeDfs->LookUp(from->GetName());
        assert(fromDf != nullptr && "From type's definition should not be null");
        if (fromDf->GetBaseType()->Compare(*to)) return to;
        // 4. otherwise check if "from" type contains cast to "to" type in typeDf
        auto casts = fromDf->GetCastsEnv();
        if (casts->HasItem(to->GetName())) return to;

        Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
        errManager.TypeCastNotPossible(filename, from, to, loc, prefix);
        return std::make_shared<ValType>("void");
    }
}

