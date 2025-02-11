
#include <filesystem>
#include <memory>
#include "../../include/typechecker.h"
#include "include/iterator.h"
#include "include/node/function.h"

namespace fs = std::filesystem;

namespace Runtime
{
    using namespace AST::Nodes;

    void TypeChecker::RunModuleChecker(std::shared_ptr<RequireMacro> require)
    {
        std::string module = require->GetModule();
        fs::path currentPath = GetFilename();
        fs::path modulePath = currentPath.parent_path() / (module + ".tmpl");

        if (m_modules->HasItem(modulePath.string()))
            return;

        std::ifstream moduleFile(modulePath);
        if (!moduleFile.good())
        {
            Prelude::ErrorManager& errManager = Prelude::ErrorManager::getInstance();
            errManager.FailedOpeningFile(modulePath.string());
            ReportError();
            return;
        }

        std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(moduleFile, modulePath.string());
        lexer->Tokenize();

        std::shared_ptr<Parser> parser = std::make_shared<Parser>(lexer);
        parser->Parse();

        std::shared_ptr<ProgramNode> program = std::dynamic_pointer_cast<ProgramNode>(parser->GetRoot());

        SetFilename(modulePath.string());
        HandleModule(program);
        SetFilename(currentPath.string());

        m_modules->AddItem(modulePath.string(), std::make_shared<std::string>(currentPath.string()));
    }

    void TypeChecker::HandleExportStatement(std::shared_ptr<ExportStatement> exportStmt)
    {
        auto target = exportStmt->GetTarget();
        switch(target->GetType())
        {
           case NodeType::FnDecl:
                HandleFnDeclaration(std::dynamic_pointer_cast<FunctionDeclaration>(target), true);
                break;
            case NodeType::VarDecl:
                HandleVarDeclaration(std::dynamic_pointer_cast<VarDeclaration>(target));
                break;
            default:
                assert(false && "Unreachable. Should be handled by parser.");
                return;
        }

    }

    void TypeChecker::HandleModule(std::shared_ptr<ProgramNode> program)
    {
        auto it = std::make_shared<Common::Iterator>(program->Size());
        while (it->HasItems())
        {
            auto stmt = program->GetItem(it->GetPosition());
            it->Next();
            switch (stmt->GetType())
            {
                case NodeType::Require:
                    RunModuleChecker(std::dynamic_pointer_cast<RequireMacro>(stmt));
                    break;
                case NodeType::Extern:
                {
                    auto ext = std::dynamic_pointer_cast<ExternMacro>(stmt);
                    HandleFnSignature(ext->GetFnSignature(), false);
                    break;
                }
                case NodeType::Export:
                    HandleExportStatement(std::dynamic_pointer_cast<ExportStatement>(stmt));
                    break;
                case NodeType::FnDecl:
                    HandleFnDeclaration(std::dynamic_pointer_cast<FunctionDeclaration>(stmt), false);
                    break;
                default:
                    {
                        Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                        errorManager.RaiseError("Unsupported node type by evaluating imported module: " + std::to_string((int)stmt->GetType()), "TypeError");
                        ReportError();
                        break;
                    }
            }
        }
    }
}

