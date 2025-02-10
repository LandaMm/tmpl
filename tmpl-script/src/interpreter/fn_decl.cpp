

#include <memory>
#include "../../include/interpreter.h"
#include "../../include/node/statement.h"
#include "../../include/typechecker.h"
#include "include/iterator.h"
#include "include/node/function.h"

namespace Runtime
{
    using namespace AST::Nodes;

    void Interpreter::EvaluateFunctionDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl, bool exported, bool externed)
    {
        std::string name = fnDecl->GetName();
        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();
        ValueType retType = TypeChecker::EvaluateType(GetFilename(), fnDecl->GetReturnType());

        std::shared_ptr<Fn> fn = std::make_shared<Fn>(body, retType, GetFilename(), exported, externed, fnDecl->GetLocation());

        auto it = std::make_shared<Common::Iterator>(fnDecl->GetSize());
        while (it->HasItems())
        {
            auto param = fnDecl->GetItem(it->GetPosition());
            it->Next();
            ValueType paramType = TypeChecker::EvaluateType(GetFilename(), param->GetType());
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
        }

        m_functions->AddItem(name, fn);
    }
}

