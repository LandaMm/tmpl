

#include "../../include/typechecker.h"
#include "include/interpreter/value.h"

namespace Runtime
{
    using namespace AST::Nodes;

    void TypeChecker::HandleVarDeclaration(std::shared_ptr<VarDeclaration> varDecl)
    {
		PValType varType = EvaluateType(GetFilename(), varDecl->GetType(), m_type_definitions, "TypeError", this);
		std::string varName = *varDecl->GetName();
		PValType varValueType = DiagnoseNode(varDecl->GetValue());

        if (!varType->Compare(*varValueType))
        {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.VarMismatchType(GetFilename(), varName, varValueType, varType, varDecl->GetLocation(), "TypeError");
            ReportError();
			return;
        }

		std::shared_ptr<TypeVariable> var = std::make_shared<TypeVariable>(varType, varDecl->Editable());

        if (m_variables->Contains(varName))
        {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.VarAlreadyExists(GetFilename(), varName, varDecl->GetLocation(), "TypeError");
            ReportError();
			return;
        }

		m_variables->AddItem(varName, var);
    }
}

