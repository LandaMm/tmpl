
#include <filesystem>
#include <fstream>
#include <memory>
#include <cassert>
#include "../../include/interpreter.h"
#include "include/node/type.h"

namespace fs = std::filesystem;

namespace Runtime
{
    using namespace AST::Nodes;

    void Interpreter::EvaluateExportStatement(std::shared_ptr<ExportStatement> exportStmt)
    {
        auto target = exportStmt->GetTarget();
        switch(target->GetType())
        {
           case NodeType::FnDecl:
                EvaluateFunctionDeclaration(std::dynamic_pointer_cast<FunctionDeclaration>(target), true, false);
                break;
            case NodeType::VarDecl:
                EvaluateVariableDeclaration(std::dynamic_pointer_cast<VarDeclaration>(target));
                break;
            case NodeType::TypeDf:
                EvaluateTypeDefinition(std::dynamic_pointer_cast<TypeDfNode>(target), true);
                break;
            default:
                assert(false && "Unreachable. Should be handled by parser.");
                return;
        }
    }

    void Interpreter::ImportModule(std::shared_ptr<RequireMacro> require)
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
            return;
        }

        std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(moduleFile, modulePath.string());
        lexer->Tokenize();

        std::shared_ptr<Parser> parser = std::make_shared<Parser>(lexer);
        parser->Parse();

        std::shared_ptr<ProgramNode> program = std::dynamic_pointer_cast<ProgramNode>(parser->GetRoot());

        SetFilename(modulePath.string());
        EvaluateModule(program);
        SetFilename(currentPath.string());

        m_modules->AddItem(modulePath.string(), std::make_shared<std::string>(currentPath.string()));
    }
}

