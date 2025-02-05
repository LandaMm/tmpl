
#include "../../include/typechecker.h"

namespace Runtime
{
    using namespace AST::Nodes;

    ValueType TypeChecker::DiagnoseLiteral(std::shared_ptr<LiteralNode> literal)
    {
        switch(literal->GetLiteralType())
        {
            case LiteralType::INT:
                return ValueType::Integer;
            case LiteralType::DOUBLE:
                return ValueType::Double;
            case LiteralType::FLOAT:
                return ValueType::Float;
            case LiteralType::STRING:
                return ValueType::String;
            case LiteralType::BOOL:
                return ValueType::Bool;
            default:
                break;
        }

        assert(literal->GetLiteralType() != LiteralType::_NULL &&
                "Literal type shouldn't be left null");
        assert(false && "Unreachable. Should handle all literal types");

        return ValueType::Null;
    }
}

