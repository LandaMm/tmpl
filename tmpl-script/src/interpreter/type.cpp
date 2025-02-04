
#include "../../include/interpreter.h"

namespace Runtime
{
    using namespace AST::Nodes;

	ValueType Interpreter::EvaluateType(std::shared_ptr<Node> typeNode)
	{
		if (typeNode->GetType() == NodeType::Identifier)
		{
			auto id = std::dynamic_pointer_cast<IdentifierNode>(typeNode);
			if (id->GetName() == "string")
			{
				return ValueType::String;
			}
			else if (id->GetName() == "double")
			{
				return ValueType::Double;
			}
			else if (id->GetName() == "float")
			{
				return ValueType::Float;
			}
			else if (id->GetName() == "int")
			{
				return ValueType::Integer;
			}
			else
			{
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.UndefinedType(GetFilename(), id->GetName(), id->GetLocation());
			}
		}
		else
		{
			// TODO: support for complex types, e.g. inline objects, generic types
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported node type for type: " + std::to_string((int)typeNode->GetType()));
		}

		return ValueType::Null;
	}
}

