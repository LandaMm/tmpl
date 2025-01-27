
#include "../../include/interpreter/environment.h"

namespace Runtime
{
	template <typename T>
	bool Environment<T>::HasVariable(std::string name)
	{
		if (m_declarations.find(name) == m_declarations.end())
		{
			if (m_parent != nullptr)
				return m_parent->HasVariable(name);
			else
				return false;
		}

		return true;
	}

	template <typename T>
	std::shared_ptr<T> Environment<T>::LookUp(std::string name)
	{
		if (m_declarations.find(name) == m_declarations.end())
		{
			if (m_parent != nullptr)
				return m_parent->LookUp(name);
			else
				return nullptr;
		}
		else
			return m_declarations.at(name);
	}

	template <typename T>
	void Environment<T>::AddVariable(std::string name, std::shared_ptr<T> var)
	{
		// TODO: raise error when var exists already
		m_declarations.insert(std::make_pair(name, var));
	}
}
