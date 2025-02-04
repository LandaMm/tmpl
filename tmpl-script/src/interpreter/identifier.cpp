
#include "../../include/interpreter.h"

namespace Runtime
{
    using namespace AST::Nodes;

	std::shared_ptr<Value> Interpreter::EvaluateIdentifier(std::shared_ptr<IdentifierNode> identifier)
	{
		if (!m_variables->HasItem(identifier->GetName()))
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.UndeclaredVariable(GetFilename(), identifier);
			return nullptr;
		}
		std::shared_ptr<Variable> var = m_variables->LookUp(identifier->GetName());
		return var->GetValue();
	}
}

