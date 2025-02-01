
#include "../../include/interpreter.h"

namespace Runtime
{
    using namespace AST::Nodes;

    std::shared_ptr<Value> Interpreter::EvaluateReturn(std::shared_ptr<ReturnNode> ret)
    {
        return Execute(ret->GetValue());
    }
}

