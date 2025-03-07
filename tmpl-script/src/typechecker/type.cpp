

#include "../../include/typechecker.h"
#include "include/interpreter/value.h"
#include "include/iterator.h"
#include <error.h>
#include <memory>

namespace Runtime
{
    using namespace AST::Nodes;

    PValType TypeChecker::GetRootType(std::string filename, PValType target, Location loc, TypeChecker::PTypeDfs typeDfs, std::string prefix)
    {
        auto typ = target;

        while (typ != nullptr)
        {
            if (!typeDfs->HasItem(typ->GetName()))
            {
                Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
                errManager.TypeDoesNotExist(filename, typ, loc, prefix);
                return std::make_shared<ValType>("void");
            }

            auto typDf = typeDfs->LookUp(typ->GetName());
            assert(typDf != nullptr && "Found typ df should not be nullptr");

            if (typDf->GetBaseType() == nullptr)
                break;
            else
                typ = typDf->GetBaseType();
        }

        return typ;
    }

    PValType TypeChecker::EvaluateType(std::string filename, std::shared_ptr<TypeNode> typeNode, TypeChecker::PTypeDfs typeDfs, std::string prefix, TypeChecker* typChecker)
    {
        // check for type existance
        std::string typName = typeNode->GetTypeName()->GetName();
        auto typ = std::make_shared<ValType>(typName);
        typ = NormalizeType(filename, typ, typeNode->GetLocation(), typeDfs, prefix, typChecker);

        if (!typeDfs->HasItem(typName))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(filename, typName, typeNode->GetLocation(), prefix);
            if (typChecker != nullptr) typChecker->ReportError();
            return std::make_shared<ValType>("void");
        }

        auto typDf = typeDfs->LookUp(typName);
        assert(typDf != nullptr && "Found typ df should not be nullptr");

        if (typDf->GetModuleName() != filename && !typDf->IsExported())
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.PrivateTypeError(filename, typName, typDf->GetModuleName(), typeNode->GetLocation(), typDf->GetLocation(), prefix);
            if (typChecker != nullptr) typChecker->ReportError();
            return std::make_shared<ValType>("void");
        }

        if (typeNode->GetGenericsSize() != typDf->GenericsSize())
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeGenericsExhausted(filename, typName, typeNode->GetGenericsSize(), typDf->GenericsSize(), typeNode->GetLocation(), prefix);
            if (typChecker != nullptr) typChecker->ReportError();
            return std::make_shared<ValType>("void");
        }

        auto pTyp = std::make_shared<ValType>(typName);

        auto it = std::make_shared<Common::Iterator>(typeNode->GetGenericsSize());
        while (it->HasItems())
        {
            auto genNode = typeNode->GetGeneric(it->GetPosition());
            it->Next();
            pTyp->AddGeneric(EvaluateType(filename, genNode, typeDfs, prefix, typChecker));
        }

        if (typDf->IsTransparent())
        {
            pTyp->SetName(typDf->GetBaseType()->GetName());
        }

        return pTyp;
    }

    PValType TypeChecker::NormalizeType(std::string filename, PValType target, Location loc, TypeChecker::PTypeDfs typeDfs, std::string prefix, TypeChecker* typChecker)
    {
        // clone
        auto typ = std::make_shared<ValType>(target);
        // check for type existance
        if (!typeDfs->HasItem(typ->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(filename, typ->GetName(), loc, prefix);
            if (typChecker != nullptr) typChecker->ReportError();
            return std::make_shared<ValType>("void");
        }

        auto typDf = typeDfs->LookUp(typ->GetName());
        assert(typDf != nullptr && "Found typ df should not be nullptr");

        if (typDf->GetModuleName() != filename && !typDf->IsExported())
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.PrivateTypeError(filename, typ->GetName(), typDf->GetModuleName(), loc, typDf->GetLocation(), prefix);
            if (typChecker != nullptr) typChecker->ReportError();
            return std::make_shared<ValType>("void");
        }

        if (typDf->IsTransparent())
        {
            typ->SetName(typDf->GetBaseType()->GetName());
        }

        return typ;
    }

    PValType TypeChecker::CastType(std::string filename, PValType from, PValType to, Location loc, TypeChecker::PTypeDfs typeDfs, std::string prefix, TypeChecker* typChecker)
    {
        auto fromTyp = 
            NormalizeType(filename, from, loc, typeDfs, prefix, typChecker);

        if (fromTyp->IsMixed()) return to;
        if (to->IsMixed()) return to;

        std::cout << "[DEBUG] from normalize: (" << *from << ") -> (" << *fromTyp << ")" << std::endl;
        // 0. check if "from" type exists in typeDfs
        //
        // should not be triggered
        if (!typeDfs->HasItem(fromTyp->GetName()))
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.TypeDoesNotExist(filename, fromTyp, loc, prefix);
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
        if (fromTyp->Compare(*to)) return to;
        // 3. check if "from" type's name is "to" type's basename
        auto fromDf = typeDfs->LookUp(fromTyp->GetName());
        assert(fromDf != nullptr && "From type's definition should not be null");
        if (fromDf->GetBaseType()->Compare(*to)) return to;
        // 4. otherwise check if "from" type contains cast to "to" type in typeDf
        auto casts = fromDf->GetCastsEnv();
        if (casts->HasItem(to->GetName())) return to;

        Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
        errManager.TypeCastNotPossible(filename, fromTyp, to, loc, prefix);
        return std::make_shared<ValType>("void");
    }
}

