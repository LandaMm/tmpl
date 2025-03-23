
#ifndef IR_BINARY_H_
#define IR_BINARY_H_

#include "include/ir.h"
namespace IR
{
    enum class BinaryOp
    {
        Add, Sub,
        Mul, Div,
        And, Or
    };

    class BinaryInst : public IRInst, public IRTemp
    {
    private:
        BinaryOp m_op;
        shared_ptr<IRTemp> m_left;
        shared_ptr<IRTemp> m_right;
    public:
        BinaryInst(tid id, shared_ptr<IRTemp> left, BinaryOp op, shared_ptr<IRTemp> right)
            : IRInst(InstType::Binary), IRTemp(id),
              m_left(left), m_op(op), m_right(right) { }
    public:
        inline BinaryOp GetOp() const { return m_op; }
        inline shared_ptr<IRTemp> GetLeft() const { return m_left; }
        inline shared_ptr<IRTemp> GetRight() const { return m_right; }
    public:
        virtual std::string Format() const override;
    };
}

#endif // IR_BINARY_H_

