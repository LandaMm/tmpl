#pragma once

#include<iostream>
#include "cpl-basics/array.hpp"
#include "cpl-basics/string.hpp"

namespace Runtime
{
    class CliRunner
    {
    public:
        CliRunner(int argc, char* argv[]);
        ~CliRunner() = default;

        CliRunner(const CliRunner&) = delete;
        CliRunner& operator=(const CliRunner&) = delete;
        CliRunner(CliRunner&&) = delete;
        CliRunner& operator=(CliRunner&&) = delete;
    private:
        void CheckBasicCommands();
    public:
        String GetScriptFilename();
        String GetProcedureName();
        Array<String> GetProcedureArgs();
        void ShowUsage();
    private:
        int m_argc;
        char** m_argv;
    };
}

