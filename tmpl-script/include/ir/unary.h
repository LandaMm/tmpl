
#ifndef IR_UNARY_H_
#define IR_UNARY_H_

#include "include/ir.h"

namespace IR
{
    class NegInst : public IRInst, public IRTemp
    {
    private:
        shared_ptr<IRTemp> m_target;
    public:
        NegInst(tid id, shared_ptr<IRTemp> target)
            : IRInst(InstType::Neg), IRTemp(id, target->GetDataType()) { }
    public:
        inline shared_ptr<IRTemp> GetTarget() const { return m_target; }
    public:
        virtual std::string Format() const override;
    };

    class NotInst : public IRInst, public IRTemp
    {
    private:
        shared_ptr<IRTemp> m_target;
    public:
        NotInst(tid id, shared_ptr<IRTemp> target)
            : IRInst(InstType::Not), IRTemp(id, target->GetDataType()) { }
    public:
        inline shared_ptr<IRTemp> GetTarget() const { return m_target; }
    public:
        virtual std::string Format() const override;
    };
}

#endif // IR_UNARY_H_

