
#include"../../include/node/program.h"

namespace AST
{
	namespace Nodes
	{
		std::string ProgramNode::Format() const
		{
			return "ProgramNode(" + std::to_string(m_statements.size()) + " statements)";
		}

		void ProgramNode::AddStatement(std::shared_ptr<Node> statement)
		{
			m_statements.push_back(statement);
		}
	}
}
