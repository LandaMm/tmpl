
#include"include/ir.h"

namespace IR
{
    enum class ConstType
    {
        // Int
        I8,
        I16,
        I32,
        I64,
        I128,
        // Float
        F32,
        F64
    };

    class LoadConst : public IRInst, public IRTemp
    {
    private:
        ConstType m_type;
        shared_ptr<void> m_value;
    public:
        LoadConst(tid id, ConstType typ, shared_ptr<void> value)
            : IRInst(InstType::LoadConst), IRTemp(id),
              m_type(typ), m_value(value) { }
    public:
        inline ConstType GetType() const { return m_type; }
        inline shared_ptr<void> GetValue() const { return m_value; }
    public:
        virtual std::string Format() const override;
    };
}

