
#include"../../include/node/statement.h"
#include <string>

namespace AST
{
	namespace Statements
	{
        std::string StatementsNode::Format() const
        {
            return "Statements(" + std::to_string(m_body.size()) + " stmts)";
        }

		void StatementsNode::AddItem(std::shared_ptr<Node> item)
		{
			m_body.push_back(item);
		}

        std::string IfElseStatement::Format() const
        {
            return "IfElse(" + m_condition->Format() + ", " + m_else_statement->Format() + ")";
        }
	}
}
