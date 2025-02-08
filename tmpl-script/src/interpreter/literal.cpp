
#include <cassert>
#include "../../include/interpreter.h"

namespace Runtime
{
    using namespace AST::Nodes;

	std::shared_ptr<Value> Interpreter::EvaluateLiteral(std::shared_ptr<LiteralNode> literal)
	{
		switch (literal->GetLiteralType())
		{
		case LiteralType::INT:
			return std::make_shared<IntegerValue>(*literal->GetValue<int>());
		case LiteralType::FLOAT:
			return std::make_shared<FloatValue>(*literal->GetValue<float>());
		case LiteralType::DOUBLE:
			return std::make_shared<DoubleValue>(*literal->GetValue<double>());
		case LiteralType::STRING:
			return std::make_shared<StringValue>(*literal->GetValue<std::string>());
		default:
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported literal type: " + std::to_string((int)literal->GetLiteralType()), "RuntimeError");
			break;
		}

        assert(false && "Unreachable code. Evaluate literal");

		return nullptr;
	}
}

