
#include "include/ir/unary.h"
#include "include/ir.h"
#include "include/node/unary.h"

namespace IR
{
    std::string NegInst::Format() const
    {
        return IRTemp::Format() + " = neg_" + FormatDataType(GetDataType()) + " " + GetTarget()->Format();
    }

    std::string NotInst::Format() const
    {
        return IRTemp::Format() + " = not_" + FormatDataType(GetDataType()) + " " + GetTarget()->Format();
    }

    shared_ptr<IRTemp> IRParser::ParseUnary(shared_ptr<UnaryNode> unary)
    {
        auto target = ParseNodeWithValue(unary->GetTarget());
        if (unary->GetOperator() == UnaryNode::UnaryOperator::Positive)
            return target;

        switch(unary->GetOperator())
        {
            case UnaryNode::UnaryOperator::Negative:
                return std::make_shared<NegInst>(GetTID(), target);
            case UnaryNode::UnaryOperator::Not:
                return std::make_shared<NegInst>(GetTID(), target);
            default:
                assert(false && "should be unreachable");
                return nullptr;
        }
    }
}

