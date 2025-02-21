
#include "../../include/typechecker.h"

namespace Runtime
{
    using namespace AST::Nodes;

    PValType TypeChecker::DiagnoseLiteral(std::shared_ptr<LiteralNode> literal)
    {
        switch(literal->GetLiteralType())
        {
            case LiteralType::INT:
                return std::make_shared<ValType>("int");
            case LiteralType::DOUBLE:
                return std::make_shared<ValType>("double");
            case LiteralType::FLOAT:
                return std::make_shared<ValType>("float");
            case LiteralType::STRING:
                // TODO: return list<char>
                return std::make_shared<ValType>("string");
            case LiteralType::BOOL:
                return std::make_shared<ValType>("bool");
            default:
                break;
        }

        assert(literal->GetLiteralType() != LiteralType::_NULL &&
                "Literal type shouldn't be left null");

        assert(false && "Unreachable. Should handle all literal types");
        return nullptr;
    }
}

