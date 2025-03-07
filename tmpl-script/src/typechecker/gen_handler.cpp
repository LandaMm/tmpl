

#include "include/iterator.h"
#include "include/typechecker.h"

namespace Runtime
{
    GenHandler::GenHandler(std::string filename, TypeChecker::PTypeDfs& typDfs, std::string prefix, TypeChecker* typChecker)
    {
        m_parent_dfs = typDfs;
        m_filename = filename;
        m_prefix = prefix;
        m_typchecker = typChecker;

        m_generic_dfs = std::make_shared<TypeChecker::TypeDfs>(m_parent_dfs);
        typDfs = m_generic_dfs;
    }

    TypeChecker::PTypeDfs GenHandler::Unload()
    {
        assert(m_generic_dfs->GetParent() != nullptr && "Typ dfs parent gone.");
        assert(m_generic_dfs->GetParent() != m_generic_dfs && "Parent clone");
        assert(m_generic_dfs->GetParent() == m_parent_dfs && "Parent was replaced");
        m_generic_dfs = nullptr;
        return m_parent_dfs;
    }
}

