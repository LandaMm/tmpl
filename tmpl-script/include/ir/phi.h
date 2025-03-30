
#ifndef _IR_PHI_H_
#define _IR_PHI_H_

#include "include/ir.h"

namespace IR
{
    class PhiInst : public IRInst, public IRTemp
    {
    private:
        shared_ptr<IRTemp> m_left; // TODO: store origin block
        shared_ptr<IRTemp> m_right; // TODO: store origin block
    public:
        PhiInst(tid id, shared_ptr<IRTemp> left, shared_ptr<IRTemp> right)
            : IRInst(InstType::Phi), IRTemp(id, left->GetDataType()), // TODO: better data type find out
              m_left(left), m_right(right) { }
    public:
        inline shared_ptr<IRTemp> GetLeft() const { return m_left; }
        inline shared_ptr<IRTemp> GetRight() const { return m_right; }
    public:
        virtual std::string Format() const override;
    };
}

#endif // _IR_PHI_H_

