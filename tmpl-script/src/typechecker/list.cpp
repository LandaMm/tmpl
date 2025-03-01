
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    PValType TypeChecker::DiagnoseList(std::shared_ptr<ListNode> list)
    {
        auto itemType = EvaluateType(GetFilename(), list->GetItemsType(), m_type_definitions, "TypeError", this);

        // check all items type
        auto it = Common::Iterator(list->Size());
        while(it.HasItems())
        {
            auto item = list->GetItem(it.GetPosition());
            it.Next();
            auto iType = DiagnoseNode(item);
            if (!itemType->Compare(*iType))
            {
                Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
                errManager.ListItemMismatchType(GetFilename(), iType, itemType, item->GetLocation(), "TypeError");
                return std::make_shared<ValType>("void");
            }
        }

        auto listType = std::make_shared<ValType>("list");

        listType->AddGeneric(itemType);

        return listType;
    }
}

