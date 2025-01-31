

#include"../include/cli.h"
#include"../include/error.h"
#include<filesystem>

namespace Runtime
{
    std::string CliRunner::GetScriptFilename()
    {
        Prelude::ErrorManager& errMan = Prelude::ErrorManager::getInstance();
        // example: tmpl <script_name> [procedure]
        if (m_argc < 2)
        {
            // TODO: Show usage example.
            errMan.NotEnoughArgs(1, m_argc - 1, true);
            return "";
        }

        std::string filename = m_argv[1];

        if (filename.find('/') > -1)
        {
            errMan.InvalidArgument(filename, "Script name cannot contain path special characters lile '/'. Please use script basename.");
            return "";
        }

        auto cwd = std::filesystem::current_path();

        std::filesystem::path scriptFilename = cwd / filename;

        return scriptFilename.string();
    }

    std::string CliRunner::GetProcedureName()
    {
        Prelude::ErrorManager& errMan = Prelude::ErrorManager::getInstance();
        // example: tmpl <script_name> [procedure]
        if (m_argc < 3)
        {
            // TODO: Show usage example.
            errMan.NotEnoughArgs(2, m_argc - 1, false);
        }

        std::string procedure = m_argv[2];

        return procedure;
    }

    std::vector<std::string> CliRunner::GetProcedureArgs()
    {
        Prelude::ErrorManager& errMan = Prelude::ErrorManager::getInstance();
        // example: tmpl <script_name> [procedure] [...args]
        if (m_argc < 4)
        {
            // TODO: Show usage example.
            errMan.NotEnoughArgs(3, m_argc - 1, true);
        }

        std::vector<std::string> procArgs;

        for (int i = 3; i < m_argc - 3; i++)
        {
            procArgs.push_back(m_argv[i]);
        }

        return procArgs;
    }
}

