
#ifndef _IR_BLOCK_H_
#define _IR_BLOCK_H_

#include "include/ir.h"

namespace IR
{
    class BlockInst : public IRInst, public IRWriter
    {
    private:
        shared_ptr<IRLabel> m_label;
        std::vector<shared_ptr<IRInst>> m_body;
    public:
        BlockInst(shared_ptr<IRLabel> label)
            : IRInst(InstType::BasicBlock),
              m_label(label) { }
    public:
        inline shared_ptr<IRLabel> GetLabel() const { return m_label; }
    public:
        inline unsigned long GetSize() const { return m_body.size(); }
        inline shared_ptr<IRInst> GetInst(unsigned long index) const { return m_body[index]; }
    public:
        virtual void WriteInst(shared_ptr<IRInst> inst) override { m_body.push_back(inst); }
    public:
        virtual std::string Format() const override;
    };
}

#endif // _IR_BLOCK_H_

