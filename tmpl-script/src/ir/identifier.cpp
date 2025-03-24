
#include "include/ir/identifier.h"
#include "include/ir.h"

namespace IR
{
    std::string LoadInst::Format() const
    {
        return IRTemp::Format() + " = load " + m_varName;
    }

    shared_ptr<IRTemp> IRParser::ParseIdentifier(shared_ptr<IdentifierNode> id)
    {
        auto inst = std::make_shared<LoadInst>(GetTID(), id->GetName());

        // Search for variable type
        DataType* datType = GetVarReturnType(id->GetName());
        if (datType == nullptr)
        {
            std::cerr << "[ParseIdentifier] Unknown type for variable: "
                << id->GetName() << std::endl;
            std::exit(1);
        }

        inst->SetDataType(*datType);

        AddInst(inst);
        return inst;
    }
}

