
#include <memory>
#include "include/ir.h"
#include "include/ir/fncall.h"
#include "include/iterator.h"
#include "include/node.h"
#include "include/node/identifier.h"

namespace IR
{
    std::string CallInst::Format() const
    {
        std::string formatted = IRTemp::Format() + " = call " + FormatDataType(GetDataType()) + " " + m_fnName + "(";

        for(auto it = Common::Iterator(m_args.size()); it.HasItems(); it.Next())
        {
            auto arg = m_args[it.GetPosition()];
            formatted += arg->Format();
            if (it.GetPosition() + 1 < m_args.size())
            {
                formatted += ", ";
            }
        }

        formatted += ")";

        return formatted;
    }

    shared_ptr<IRTemp> IRParser::ParseFunctionCall(shared_ptr<FunctionCall> fnCall)
    {
        assert(fnCall->GetCallee()->GetType() == AST::NodeType::Identifier && "Only identifiers are supported for calls");
        auto id = std::dynamic_pointer_cast<IdentifierNode>(fnCall->GetCallee());
        auto inst = std::make_shared<CallInst>(GetTID(), id->GetName());

        // Search for function return data type
        DataType* retType = GetFnReturnType(id->GetName());
        if (retType == nullptr)
        {
            std::cerr << "[ParseFunctionCall] Unknown return type for function: "
                << id->GetName() << std::endl;
            std::exit(1);
        }

        inst->SetDataType(*retType);

        // Args
        for(auto it = Common::Iterator(fnCall->GetArgumentsSize()); it.HasItems(); it.Next())
        {
            auto arg = fnCall->GetArgument(it.GetPosition());
            auto arg_value = ParseNodeWithValue(arg);
            inst->AddArgument(arg_value);
        }

        AddInst(inst);
        return inst;
    }
}

