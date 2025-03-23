
#include "include/ir/binary.h"
#include "include/ir.h"
#include "include/node/expression.h"
#include <string>

namespace IR
{
    std::string BinaryInst::Format() const
    {
        std::string formatted = IRTemp::Format() + " = " + GetLeft()->Format() + " ";

        switch(GetOp())
        {
            case BinaryOp::Add: formatted += "+"; break;
            case BinaryOp::Sub: formatted += "-"; break;
            case BinaryOp::Mul: formatted += "*"; break;
            case BinaryOp::Div: formatted += "/"; break;
            case BinaryOp::And: formatted += "&&"; break;
            case BinaryOp::Or: formatted += "||"; break;
        }

        formatted += " " + GetRight()->Format();
        return formatted;
    }

    shared_ptr<IRTemp> IRParser::ParseExpression(shared_ptr<ExpressionNode> expr)
    {
        auto left = ParseNodeWithValue(expr->GetLeft());
        auto right = ParseNodeWithValue(expr->GetRight());
        
        BinaryOp op;

        switch(expr->GetOperator().GetType())
        {
            case ExpressionNode::OperatorType::PLUS:
                op = BinaryOp::Add;
                break;
            case ExpressionNode::OperatorType::MINUS:
                op = BinaryOp::Sub;
                break;
            case ExpressionNode::OperatorType::MULTIPLY:
                op = BinaryOp::Mul;
                break;
            case ExpressionNode::OperatorType::DIVIDE:
                op = BinaryOp::Div;
                break;
            default:
                assert(false && "Should be unreachable");
                return nullptr;
        }

        auto inst = std::make_shared<BinaryInst>(GetTID(), left, op, right);
        AddInst(inst);
        return inst;
    }
}

