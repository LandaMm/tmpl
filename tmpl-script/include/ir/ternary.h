
#ifndef _IR_TERNARY_H_
#define _IR_TERNARY_H_

#include "include/ir.h"

namespace IR
{
    class BranchInst : public IRInst
    {
    private:
        shared_ptr<IRTemp> m_target;
        shared_ptr<IRLabel> m_true_block;
        shared_ptr<IRLabel> m_false_block;
    public:
        BranchInst(shared_ptr<IRTemp> target, shared_ptr<IRLabel> trueBlock, shared_ptr<IRLabel> falseBlock)
            : IRInst(InstType::Br),
              m_target(target),
              m_true_block(trueBlock),
              m_false_block(falseBlock) { }
    public:
        inline shared_ptr<IRTemp> GetTarget() const { return m_target; }
        inline shared_ptr<IRLabel> GetTrueBLabel() const { return m_true_block; }
        inline shared_ptr<IRLabel> GetFalseBLabel() const { return m_false_block; }
    public:
        virtual std::string Format() const override;
    };

    class JmpInst : public IRInst
    {
    private:
        shared_ptr<IRLabel> m_jmp_id;
    public:
        JmpInst(shared_ptr<IRLabel> jmpId)
            : IRInst(InstType::Jmp),
              m_jmp_id(jmpId) { }
    public:
        inline shared_ptr<IRLabel> GetJmpLabel() const { return m_jmp_id; }
    public:
        virtual std::string Format() const override;
    };
}

#endif // _IR_TERNARY_H_

