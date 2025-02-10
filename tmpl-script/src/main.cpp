
#include <cassert>
#include <iostream>
#include <memory>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interpreter.h"
#include "../include/interpreter/environment.h"
#include "../include/interpreter/value.h"
#include "../include/cli.h"
#include "../include/error.h"
#include "../include/typechecker.h"

int main(int argc, char **argv)
{
	using namespace AST;
	using namespace Runtime;

    CliRunner cliRunner(argc, argv);

    std::string filename = cliRunner.GetScriptFilename();

	std::ifstream input(filename);

    Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();

    if (!input.good())
    {
        errManager.FailedOpeningFile(filename);
        return 1;
    }

	std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(input, filename);
	lexer->Tokenize();

	std::shared_ptr<Parser> parser = std::make_shared<Parser>(lexer);
	parser->Parse();

    std::shared_ptr<TypeChecker> typeChecker = std::make_shared<TypeChecker>(parser);

    typeChecker->RunChecker(std::dynamic_pointer_cast<ProgramNode>(parser->GetRoot()));

    if (typeChecker->GetErrorReport() > 0)
    {
        errManager.RaiseError("Found " + std::to_string(typeChecker->GetErrorReport()) + " type errors. Exiting...", "PreLaunchError:");
    }

	auto variables = std::make_shared<Environment<Variable>>();
	auto procedures = std::make_shared<Environment<Procedure>>();
	auto functions = std::make_shared<Environment<Fn>>();
	auto modules = std::make_shared<Environment<std::string>>();
    auto typeFunctions = std::make_shared<Environment<Environment<Fn>, Runtime::ValueType>>();

	Interpreter intrpt(parser, variables, procedures, functions, modules, typeFunctions);

	std::shared_ptr<ProgramNode> program = std::dynamic_pointer_cast<ProgramNode>(parser->GetRoot());

    intrpt.Evaluate(program);

    std::string procName = cliRunner.GetProcedureName();

    if (!procedures->HasItem(procName))
    {
        errManager.ProcedureNotFound(procName);
        return -1;
    }

    std::vector<std::string> args = cliRunner.GetProcedureArgs();

    std::shared_ptr<ListValue> argsList = std::make_shared<ListValue>();

    for (auto arg : args)
    {
        std::shared_ptr<StringValue> argVal = std::make_shared<StringValue>(arg);
        argsList->AddItem(argVal);
    }

    std::shared_ptr<Variable> argsVar = std::make_shared<Variable>(
        ValueType::List,
        argsList,
        false
    );
    variables->AddItem("ARGS", argsVar);

    std::shared_ptr<Procedure> procedure = procedures->LookUp(procName);
    std::shared_ptr<Value> retVal = intrpt.Execute(procedure->GetBody());
    std::cout << "[DEBUG] evaluated procedure: " << retVal << std::endl;

	return 0;
}



