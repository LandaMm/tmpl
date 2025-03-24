
#include "include/ir.h"
#include "include/ir/literal.h"

namespace IR
{
    enum class CmpType
    {
        Eq, Neq,
        Lt, Gt,
        Le, Ge
    };

    class CmpInst : public IRInst, public IRTemp
    {
    private:
        CmpType m_cmpTyp;
        shared_ptr<IRTemp> m_left;
        shared_ptr<IRTemp> m_right;
    public:
        CmpInst(tid id, CmpType cmpType, shared_ptr<IRTemp> left, shared_ptr<IRTemp> right)
            : IRTemp(id, DataType::I1), IRInst(InstType::Cmp),
              m_left(left), m_right(right),
              m_cmpTyp(cmpType) { }
    public:
        inline CmpType GetCmpType() const { return m_cmpTyp; }
    public:
        virtual std::string Format() const override;
    };
}

