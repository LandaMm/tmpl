
#include "../../include/interpreter.h"

namespace Runtime
{
    using namespace AST::Nodes;

	void Interpreter::EvaluateProcedureDeclaration(std::shared_ptr<ProcedureDeclaration> procDecl)
	{
		std::string name = procDecl->GetName();
		std::shared_ptr<Statements::StatementsBody> body = procDecl->GetBody();

		std::shared_ptr<Procedure> proc = std::make_shared<Procedure>(body);
		m_procedures->AddItem(name, proc);
	}
}

