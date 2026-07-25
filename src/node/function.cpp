

#include"../../include/node/function.h"

namespace AST
{
	namespace Nodes
	{
		std::string FunctionCall::Format() const
		{
			return "FnCall(" + std::to_string(m_args.size()) + ")";
		}

		std::string FunctionDeclaration::Format() const
		{
			return "FnDecl(" + m_name->Format() + ", " + std::to_string(m_params.size()) + " params, returns " + m_ret_type->Format() + ")";
		}

        void FunctionDeclaration::AddParam(std::shared_ptr<FunctionParam> param)
        {
            m_params.push_back(param);
        }
	}
}
