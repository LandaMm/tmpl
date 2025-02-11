
#include "../../include/typechecker.h"

namespace Runtime
{
    using namespace AST::Nodes;

    std::shared_ptr<ComplexValueType> TypeChecker::DiagnoseLiteral(std::shared_ptr<LiteralNode> literal)
    {
        switch(literal->GetLiteralType())
        {
            case LiteralType::INT:
                return std::make_shared<ComplexValueType>(ValueType::Integer);
            case LiteralType::DOUBLE:
                return std::make_shared<ComplexValueType>(ValueType::Double);
            case LiteralType::FLOAT:
                return std::make_shared<ComplexValueType>(ValueType::Float);
            case LiteralType::STRING:
                return std::make_shared<ComplexValueType>(ValueType::String);
            case LiteralType::BOOL:
                return std::make_shared<ComplexValueType>(ValueType::Bool);
            default:
                break;
        }

        assert(literal->GetLiteralType() != LiteralType::_NULL &&
                "Literal type shouldn't be left null");
        assert(false && "Unreachable. Should handle all literal types");

        return std::make_shared<ComplexValueType>(ValueType::Null);
    }
}

