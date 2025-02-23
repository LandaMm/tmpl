
#include "include/interpreter.h"
#include "include/interpreter/environment.h"

namespace Runtime
{
    std::shared_ptr<Value> Interpreter::EvaluateForLoop(std::shared_ptr<ForLoopNode> forLoopNode)
    {
        auto scope = std::make_shared<Environment<Variable>>(m_variables);
        m_variables = scope;

        Execute(forLoopNode->GetDecl());

        auto condition = Execute(forLoopNode->GetCondition());
        auto condType = condition->GetType();

        assert(condType->Compare(ValType("bool")) && "Condition returned non-boolean value. Should be handled by evaluate condition method");

        auto condVal = std::dynamic_pointer_cast<BoolValue>(condition);

        m_breakStack.push_back(false);

        while (condVal->GetValue() == true)
        {
            auto val = Execute(forLoopNode->GetBody());
            if (m_breakStack.back())
            {
                break;
            }
            if (!val->GetType()->Compare(ValType("void")))
            {
                assert(scope->GetParent() != nullptr && "Scope parent gone.");
                m_variables = scope->GetParent();
                assert(m_variables != scope && "Parent and child are located at the same memory space.");
                return val;
            }

            Execute(forLoopNode->GetAssignment());

            condition = Execute(forLoopNode->GetCondition());
            assert(condition != nullptr && "While loop condition cannot return no value");
            condType = condition->GetType();
            assert(condType->Compare(ValType("bool")) && "Condition returned non-boolean value. Should be handled by evaluate condition method");
            condVal = std::dynamic_pointer_cast<BoolValue>(condition);
        }

        m_breakStack.pop_back();

        assert(scope->GetParent() != nullptr && "Scope parent gone.");
        m_variables = scope->GetParent();
        assert(m_variables != scope && "Parent and child are located at the same memory space.");

        return std::make_shared<VoidValue>();
    }
}

