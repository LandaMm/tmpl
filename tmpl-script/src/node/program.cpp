
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

		std::shared_ptr<Node> ProgramNode::Next()
		{
			if (m_index >= m_statements.size())
			{
				return nullptr;
			}

			return m_statements[m_index++];
		}

		void ProgramNode::Reset()
		{
			m_index = -1;
		}
	}
}
