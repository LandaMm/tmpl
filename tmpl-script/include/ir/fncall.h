
#include "include/ir.h"
#include <vector>

namespace IR
{
    class CallInst : public IRInst, public IRTemp
    {
    private:
        std::string m_fnName;
        std::vector<shared_ptr<IRTemp>> m_args;
    public:
        CallInst(tid id, std::string fnName)
            : IRInst(InstType::Call), IRTemp(id, DataType::Void),
              m_fnName(fnName) { }
    public:
        void AddArgument(shared_ptr<IRTemp> tmp) { m_args.push_back(tmp); }
    public:
        inline std::string GetFnName() const { return m_fnName; }
    public:
        inline unsigned int GetArgsSize() const { return m_args.size(); }
        inline shared_ptr<IRTemp> GetArgument(unsigned int index) const { return m_args[index]; }
    public:
        virtual std::string Format() const override;
    };
}

