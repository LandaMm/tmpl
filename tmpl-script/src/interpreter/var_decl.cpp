
#include "../../include/interpreter.h"
#include "include/typechecker.h"

namespace Runtime
{
    using namespace AST::Nodes;

	void Interpreter::EvaluateVariableDeclaration(std::shared_ptr<VarDeclaration> varDecl)
	{
		PValType varType = TypeChecker::EvaluateType(GetFilename(), varDecl->GetType(), m_type_definitions, "RuntimeError", nullptr);
		std::string varName = *varDecl->GetName();
		std::shared_ptr<Value> varValue = Execute(varDecl->GetValue());

		if (!varType->Compare(*varValue->GetType()))
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.VarMismatchType(GetFilename(), varName, varValue->GetType(), varType, varDecl->GetLocation(), "RuntimeError");
			return;
		}

		std::shared_ptr<Variable> var = std::make_shared<Variable>(varType, varValue, varDecl->Editable());

        if (m_variables->Contains(varName))
        {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.VarAlreadyExists(GetFilename(), varName, varDecl->GetLocation(), "RuntimeError");
			return;
        }

		m_variables->AddItem(varName, var);
	}
}

