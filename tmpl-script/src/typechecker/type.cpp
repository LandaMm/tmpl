

#include "../../include/typechecker.h"
#include "../../include/node/generic.h"
#include <memory>

namespace Runtime
{
    using namespace AST::Nodes;

    std::shared_ptr<ComplexValueType> TypeChecker::EvaluateType(std::string filename, std::shared_ptr<Node> typeNode)
    {
		if (typeNode->GetType() == NodeType::Identifier)
		{
			auto id = std::dynamic_pointer_cast<IdentifierNode>(typeNode);
			if (id->GetName() == "string")
			{
				return std::make_shared<ComplexValueType>(ValueType::String);
			}
			else if (id->GetName() == "double")
			{
				return std::make_shared<ComplexValueType>(ValueType::Double);
			}
			else if (id->GetName() == "float")
			{
				return std::make_shared<ComplexValueType>(ValueType::Float);
			}
			else if (id->GetName() == "int")
			{
				return std::make_shared<ComplexValueType>(ValueType::Integer);
			}
			else if (id->GetName() == "bool")
            {
                return std::make_shared<ComplexValueType>(ValueType::Bool);
            }
			else if (id->GetName() == "void")
            {
                return std::make_shared<ComplexValueType>(ValueType::Null);
            }
			else
			{
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.UndefinedType(filename, id->GetName(), id->GetLocation(), "TypeError");
                // Try reporting instead of exiting
                exit(-1);
			}
		}
		else if (typeNode->GetType() == NodeType::Generic)
        {
            auto generic = std::dynamic_pointer_cast<GenericNode>(typeNode);
            auto baseType = EvaluateType(filename, generic->GetTarget());
            auto complexType = EvaluateType(filename, generic->GetTypeNode());
            baseType->SetGenericType(complexType);
            return baseType;
        }
		else
		{
			// TODO: support for complex types, e.g. inline objects, generic types
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported node type for type: " + std::to_string((int)typeNode->GetType()), "TypeError");
            // TODO: try reporint instead of exiting
            exit(-1);
		}

		return std::make_shared<ComplexValueType>(ValueType::Null);
    }
}

