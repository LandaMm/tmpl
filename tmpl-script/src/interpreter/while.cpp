
#include "include/interpreter.h"

namespace Runtime
{
    std::shared_ptr<Value> Interpreter::EvaluateWhileLoop(std::shared_ptr<WhileNode> whileNode)
    {
        auto condition = Execute(whileNode->GetCondition());
        assert(condition != nullptr && "While loop condition cannot return no value");
        auto condType = condition->GetType();

        assert(condType->Compare(ValType("bool")) && "Condition returned non-boolean value. Should be handled by evaluate condition method");

        auto condVal = std::dynamic_pointer_cast<BoolValue>(condition);

        m_breakStack.push_back(false);

        while (condVal->GetValue() == true)
        {
            auto val = Execute(whileNode->GetBody());
            if (m_breakStack.back())
            {
                break;
            }
            if (!val->GetType()->Compare(ValType("void")))
            {
                return val;
            }

            condition = Execute(whileNode->GetCondition());
            assert(condition != nullptr && "While loop condition cannot return no value");
            condType = condition->GetType();
            assert(condType->Compare(ValType("bool")) && "Condition returned non-boolean value. Should be handled by evaluate condition method");
            condVal = std::dynamic_pointer_cast<BoolValue>(condition);
        }

        m_breakStack.pop_back();

        return std::make_shared<VoidValue>();
    }
}

