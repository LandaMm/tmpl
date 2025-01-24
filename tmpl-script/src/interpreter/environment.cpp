
#include"../../include/interpreter/environment.h"

namespace Runtime
{
	bool Environment::HasVariable(std::string name)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			if (m_parent != nullptr)
				return m_parent->HasVariable(name);
			else
				return false;
		}

		return true;
	}

	std::shared_ptr<Variable> Environment::LookUp(std::string name)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			if (m_parent != nullptr)
				return m_parent->LookUp(name);
			else
				return nullptr;
		}
		else
			return m_variables.at(name);
	}

	void Environment::AddVariable(std::string name, std::shared_ptr<Variable> var)
	{
		// TODO: raise error when var exists already
		m_variables.insert(std::make_pair(name, var));
	}
}
