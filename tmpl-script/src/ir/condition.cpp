
#include "include/ir.h"
#include "include/node/logical.h"
#include "include/ir/condition.h"

namespace IR
{
    std::string CmpInst::Format() const
    {
        std::string formatted = IRTemp::Format() + " = ";

        switch(m_cmpTyp)
        {
            case CmpType::Eq: formatted += "eq"; break;
            case CmpType::Neq: formatted += "neq"; break;
            case CmpType::Lt: formatted += "lt"; break;
            case CmpType::Gt: formatted += "gt"; break;
            case CmpType::Ge: formatted += "ge"; break;
            case CmpType::Le: formatted += "le"; break;
        }

        // TODO: maybe better way to get operand's data type?
        formatted += "_" + FormatDataType(m_left->GetDataType()) + " ";

        formatted += m_left->Format() + ", " + m_right->Format();

        return formatted;
    }

    shared_ptr<IRTemp> IRParser::ParseCondition(shared_ptr<Condition> cond)
    {
        CmpType cmpType;

        switch (cond->GetOperator())
        {
            case Condition::ConditionType::Compare:
                cmpType = CmpType::Eq;
                break;
            case Condition::ConditionType::NotEqual:
                cmpType = CmpType::Neq;
                break;
            case Condition::ConditionType::Less:
                cmpType = CmpType::Lt;
                break;
            case Condition::ConditionType::Greater:
                cmpType = CmpType::Gt;
                break;
            case Condition::ConditionType::LessEqual:
                cmpType = CmpType::Le;
                break;
            case Condition::ConditionType::GreaterEqual:
                cmpType = CmpType::Ge;
                break;
            default:
                assert(false && "Should be unreachable");
                return nullptr;
        }

        auto left = ParseNodeWithValue(cond->GetLeft());
        auto right = ParseNodeWithValue(cond->GetRight());

        auto inst = std::make_shared<CmpInst>(GetTID(), cmpType, left, right);
        AddInst(inst);
        return inst;
    }
}

