#include <memory>
#include "../../include/interpreter.h"
#include "../../include/interpreter/value.h"

namespace Runtime
{
    using namespace AST::Nodes;

    std::shared_ptr<Value> Interpreter::EvaluateIfElseStatement(std::shared_ptr<Statements::IfElseStatement> ifElse)
    {
        auto condition = Execute(ifElse->GetCondition());
        auto condType = condition->GetType();

        assert(condType == ValueType::Bool && "Condition returned non-boolean value. Should be handled by evaluate condition method");

        auto condVal = std::dynamic_pointer_cast<BoolValue>(condition);

        if (condVal->GetValue() == true)
        {
            return Execute(ifElse->GetBody());
        } else if (ifElse->GetElseNode() != nullptr)
        {
            return Execute(ifElse->GetElseNode());
        }

        // TODO: check if this is useful when void functions are implemented!
        assert(false && "Unreachable. Should be handled by typechecker!");

        return nullptr;
    }
}

