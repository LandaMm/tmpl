#include <print>
#include <cassert>
#include <memory>

#include <cpl-parser/lexer.h>
#include <cpl-parser/parser.h>
#include <cpl-parser/node/program.hpp>
#include <cpl-parser/cli.h>
#include <cpl-parser/error.h>
#include <cpl-basics/allocator/arena.hpp>

#include <cpl-codegen/dumper.h>

#include <cpl-codegen/codegen.h>

int main(int argc, char **argv)
{
	using namespace AST;
	using namespace Runtime;

    CliRunner cliRunner(argc, argv);

    String filename = cliRunner.GetScriptFilename();

	FileReader reader(filename.c_str());

    Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();

    if (!reader.Ok())
    {
        errManager.FailedOpeningFile(filename);
        return 1;
    }

	Lexer* lexer = new Lexer(reader, filename);
	lexer->Tokenize();

	Parser* parser = new Parser(lexer);
	parser->Parse();

    auto root = reinterpret_cast<AST::Nodes::ProgramNode*>(parser->GetRoot());
    (void)root;

    auto compiler = new IRGenerate::IR(root);

    {
		FileStreamWriter irOut("out/output.ir");
		compiler->GenerateIR();
		dump_ir(compiler, &irOut);
    }

    {
        FileStreamWriter outputFile("out/output.asm");
        auto generator = new Codegen::Generator(&outputFile, compiler);
        generator->Generate();
    }

	system("nasm -f win64 -g out/output.asm -o out/output.obj");
	system("gcc -o out/output.exe out/output.obj");

    delete parser;

    // std::string procName = cliRunner.GetProcedureName();
    // std::vector<std::string> args = cliRunner.GetProcedureArgs();
	return 0;
}
