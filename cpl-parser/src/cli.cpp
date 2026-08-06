#include <filesystem>
#include <iostream>

#include "cpl-parser/cli.h"
#include "cpl-parser/error.h"

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
                std::cout << "moth version 0.0.1" << std::endl;
                std::exit(0);
            }
            if (cmd == "--help" || cmd == "-h")
            {
                ShowUsage();
                std::exit(0);
            }
        }
    }

    String CliRunner::GetScriptFilename()
    {
        Prelude::ErrorManager& errMan = Prelude::ErrorManager::getInstance();
        // example: moth <script_name> [procedure]
        // example: moth script_file.mt
        if (m_argc < 2)
        {
            std::cout << "ArgumentsError: Not enough additional arguments provided."
                << std::endl;
            ShowUsage();
            std::exit(1);
            return "";
        }

        std::string filename = m_argv[1];
        auto cwd = std::filesystem::current_path();

        if (filename.find(".mt") != -1)
        {
            std::filesystem::path scriptFilename = cwd / filename;
            return scriptFilename.string();
        }

        std::filesystem::path scriptFilename = cwd / ".mt/" / (filename + ".mt");

        return static_cast<const char*>(scriptFilename.string().c_str());
    }

    String CliRunner::GetProcedureName()
    {
        Prelude::ErrorManager& errMan = Prelude::ErrorManager::getInstance();
        // example: moth <script_name> [procedure]
        if (m_argc < 3)
        {
            return "main";
        }

        String procedure = m_argv[2];

        return procedure;
    }

    Array<String> CliRunner::GetProcedureArgs()
    {
        Array<String> procArgs;

        Prelude::ErrorManager& errMan = Prelude::ErrorManager::getInstance();
        // example: moth <script_name> [procedure] [...args]
        if (m_argc < 4)
        {
            return procArgs;
        }

        for (int i = 3; i < m_argc; i++)
        {
            procArgs.Push(m_argv[i]);
        }

        return procArgs;
    }

    void CliRunner::ShowUsage()
    {
        std::cout << std::endl;
        std::cout << "Usage: " << m_argv[0] << " <script> [procedure] [...args]"
            << std::endl << std::endl;
        std::cout << "Script:\n" << "\tName of the script placed in `.mt` folder"
            << " or script file path" << std::endl << std::endl;
        std::cout << "Procedure:\n" << "\tName of the procedure inside script to run"
            << std::endl << std::endl;
        std::cout << "Args:\n" << "\tAddition arguments to pass into procedure"
            << std::endl << std::endl;
        std::cout << "Other options:\n"
            << "\t--version, -v\t" << "version of the moth cli" << std::endl
            << "\t--help, -h\t" << "show help message" << std::endl;
    }
}

