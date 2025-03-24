
#include "include/node/literal.h"
#include "include/ir/literal.h"
#include "include/ir.h"
#include <string>

namespace IR
{
    std::string LoadConst::Format() const
    {
        std::string formatted = IRTemp::Format() + " = ";

        formatted += FormatDataType(GetType());

        switch(GetType())
        {
            case DataType::Void:
            {
                formatted += " -";
            }
            // TODO: Handle int type correctly
            case DataType::I1:
            case DataType::I8:
            case DataType::I16:
            case DataType::I32:
            case DataType::I64:
            case DataType::I128:
            {
                auto val = std::static_pointer_cast<int>(GetValue());
                formatted += " " + std::to_string(*val);
                break;
            }
            case DataType::F32:
            {
                auto val = std::static_pointer_cast<float>(GetValue());
                formatted += " " + std::to_string(*val);
                break;
            }
            case DataType::F64:
            {
                auto val = std::static_pointer_cast<double>(GetValue());
                formatted += " " + std::to_string(*val);
                break;
            }
        }

        return formatted;
    }

    shared_ptr<IRTemp> IRParser::ParseLiteral(shared_ptr<LiteralNode> lit)
    {
        switch(lit->GetLiteralType())
        {
            case LiteralType::INT:
            {
                int val = *lit->GetValue<int>();
                // TODO: add support for int32/int64 selection
                auto inst = std::make_shared<LoadConst>(GetTID(), DataType::I32, std::make_shared<int>(val));
                AddInst(inst);
                return inst;
            }
            case LiteralType::FLOAT:
            {
                // TODO: add support for float32/float64 selection
                float val = *lit->GetValue<float>();
                auto inst = std::make_shared<LoadConst>(GetTID(), DataType::F32, std::make_shared<float>(val));
                AddInst(inst);
                return inst;
            }
            default:
                std::cerr << "[ParseLiteral] Unsupported literal type for compile"
                    << lit->Format() << std::endl;
                std::exit(1);
        }
    }
}

