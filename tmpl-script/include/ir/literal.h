
#include"include/ir.h"

namespace IR
{
    class LoadConst : public IRInst, public IRTemp
    {
    private:
        DataType m_type;
        shared_ptr<void> m_value;
    public:
        LoadConst(tid id, DataType typ, shared_ptr<void> value)
            : IRInst(InstType::LoadConst), IRTemp(id, typ),
              m_type(typ), m_value(value) { }
    public:
        inline DataType GetType() const { return m_type; }
        inline shared_ptr<void> GetValue() const { return m_value; }
    public:
        virtual std::string Format() const override;
    };
}

