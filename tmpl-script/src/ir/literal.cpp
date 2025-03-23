
#include "include/node/literal.h"
#include "include/ir/literal.h"
#include "include/ir.h"
#include <string>

namespace IR
{
    std::string LoadConst::Format() const
    {
        std::string formatted = IRTemp::Format() + " = ";

        switch(GetType())
        {
            // TODO: Handle int type correctly
            case ConstType::I8:
            {
                formatted += "i8";
                auto val = std::static_pointer_cast<int>(GetValue());
                formatted += " " + std::to_string(*val);
                break;
            }
            case ConstType::I16:
            {
                formatted += "i16";
                auto val = std::static_pointer_cast<int>(GetValue());
                formatted += " " + std::to_string(*val);
                break;
            }
            case ConstType::I32:
            {
                formatted += "i32";
                auto val = std::static_pointer_cast<int>(GetValue());
                formatted += " " + std::to_string(*val);
                break;
            }
            case ConstType::I64:
            {
                formatted += "i64";
                auto val = std::static_pointer_cast<int>(GetValue());
                formatted += " " + std::to_string(*val);
                break;
            }
            case ConstType::I128:
            {
                formatted += "i128";
                auto val = std::static_pointer_cast<int>(GetValue());
                formatted += " " + std::to_string(*val);
                break;
            }
            case ConstType::F32:
            {
                formatted += "f32";
                auto val = std::static_pointer_cast<float>(GetValue());
                formatted += " " + std::to_string(*val);
                break;
            }
            case ConstType::F64:
            {
                formatted += "f64";
                auto val = std::static_pointer_cast<float>(GetValue());
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
                // TODO: add support for int32/int64 selection
                int val = *lit->GetValue<int>();
                auto inst = std::make_shared<LoadConst>(GetTID(), ConstType::I64, std::make_shared<int>(val));
                AddInst(inst);
                return inst;
            }
            case LiteralType::FLOAT:
            {
                // TODO: add support for float32/float64 selection
                float val = *lit->GetValue<float>();
                auto inst = std::make_shared<LoadConst>(GetTID(), ConstType::F32, std::make_shared<float>(val));
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

