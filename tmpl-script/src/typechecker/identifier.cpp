
#include "../../include/typechecker.h"

namespace Runtime
{
    using namespace AST::Nodes;

    PValType TypeChecker::DiagnoseId(std::shared_ptr<IdentifierNode> identifier)
    {
		if (!m_variables->HasItem(identifier->GetName()))
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.UndeclaredVariable(GetFilename(), identifier, "TypeError");
            ReportError();
			return std::make_shared<ValType>("void");
		}
		std::shared_ptr<TypeVariable> var = m_variables->LookUp(identifier->GetName());
		return var->GetType();
    }
}

