

#ifndef CLI_H
#define CLI_H
#include<iostream>
#include<string>
#include<vector>

namespace Runtime
{

    // Parse args and help main process identify procedure needed to run
    // by operating with local `.tmpl` folder that should contain
    // all the scripts
    class CliRunner
    {
    private:
        int m_argc;
        char** m_argv;
    public:
        CliRunner(int argc, char* argv[]);
        ~CliRunner() = default;
    private:
        void CheckBasicCommands();
    public:
        std::string GetScriptFilename();
        std::string GetProcedureName();
        std::vector<std::string> GetProcedureArgs();
        void ShowUsage();
    };
}

#endif // CLI_H

