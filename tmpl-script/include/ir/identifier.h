
#ifndef IR_IDENTIFIER_H_
#define IR_IDENTIFIER_H_

#include "include/ir.h"

namespace IR
{
    class LoadInst : public IRInst, public IRTemp
    {
    private:
        std::string m_varName;
    public:
        LoadInst(tid id, std::string varName)
            : IRInst(InstType::Load), IRTemp(id, DataType::Void),
              m_varName(varName) { }
    public:
        inline std::string GetVarName() const { return m_varName; }
    public:
        virtual std::string Format() const override;
    };
}

#endif // IR_IDENTIFIER_H_

