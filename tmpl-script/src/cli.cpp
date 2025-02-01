

#include"../include/cli.h"
#include"../include/error.h"
#include <filesystem>
#include <iostream>

namespace Runtime
{
    CliRunner::CliRunner(int argc, char* argv[])
    {
        m_argc = argc;
        m_argv = argv;
        CheckBasicCommands();
    }

    void CliRunner::CheckBasicCommands()
    {
        if (m_argc == 2)
        {
            std::string cmd = m_argv[1];
            if (cmd == "--version" || cmd == "-v")
            {
                std::cout << "tmpl version 0.0.1" << std::endl;
                std::exit(0);
            }
            if (cmd == "--help" || cmd == "-h")
            {
                ShowUsage();
                std::exit(0);
            }
        }
    }

    std::string CliRunner::GetScriptFilename()
    {
        Prelude::ErrorManager& errMan = Prelude::ErrorManager::getInstance();
        // example: tmpl <script_name> [procedure]
        if (m_argc < 2)
        {
            std::cout << "ArgumentsError: Not enough additional arguments provided."
                << std::endl;
            ShowUsage();
            std::exit(1);
            return "";
        }

        std::string filename = m_argv[1];

        if (filename.find('/') > -1)
        {
            errMan.InvalidArgument(filename, "Script name cannot contain path special characters lile '/'. Please use script basename.");
            return "";
        }

        auto cwd = std::filesystem::current_path();

        std::filesystem::path scriptFilename = cwd / ".tmpl/" / (filename + ".tmpl");

        return scriptFilename.string();
    }

    std::string CliRunner::GetProcedureName()
    {
        Prelude::ErrorManager& errMan = Prelude::ErrorManager::getInstance();
        // example: tmpl <script_name> [procedure]
        if (m_argc < 3)
        {
            return "main";
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
            return std::vector<std::string>();
        }

        std::vector<std::string> procArgs;

        for (int i = 3; i < m_argc; i++)
        {
            procArgs.push_back(m_argv[i]);
        }

        return procArgs;
    }

    void CliRunner::ShowUsage()
    {
        std::cout << std::endl;
        std::cout << "Usage: " << m_argv[0] << " <script> [procedure] [...args]"
            << std::endl << std::endl;
        std::cout << "Script:\n" << "\tName of the script placed in `.tmpl` folder"
            << " or script file path" << std::endl << std::endl;
        std::cout << "Procedure:\n" << "\tName of the procedure inside script to run"
            << std::endl << std::endl;
        std::cout << "Args:\n" << "\tAddition arguments to pass into procedure"
            << std::endl << std::endl;
        std::cout << "Other options:\n"
            << "\t--version, -v\t" << "version of the tmpl cli" << std::endl
            << "\t--help, -h\t" << "show help message" << std::endl;
    }
}

