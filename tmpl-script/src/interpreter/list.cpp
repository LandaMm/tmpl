
#include "include/interpreter.h"
#include "include/interpreter/value.h"
#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    std::shared_ptr<Value> Interpreter::EvaluateList(std::shared_ptr<ListNode> list)
    {
        auto itemType = TypeChecker::EvaluateType(GetFilename(), list->GetItemsType(), m_type_definitions, "RuntimeError", nullptr);

        auto listValue = std::make_shared<ListValue>(itemType);

        // check all items type
        auto it = Common::Iterator(list->Size());
        while(it.HasItems())
        {
            auto item = list->GetItem(it.GetPosition());
            it.Next();
            auto iVal = Execute(item);
            if (!itemType->Compare(*iVal->GetType()))
            {
                Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
                errManager.ListItemMismatchType(GetFilename(), iVal->GetType(), itemType, item->GetLocation(), "TypeError");
                return std::make_shared<VoidValue>();
            }
            listValue->AddItem(iVal);
        }

        return listValue;
    }
}

