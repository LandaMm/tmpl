

#include "../../include/typechecker.h"
#include "include/iterator.h"
#include <memory>

namespace Runtime
{
    using namespace AST::Nodes;

    void TypeChecker::AssumeIfElse(std::shared_ptr<Statements::IfElseStatement> ifElse, PValType expected)
    {
        AssumeBlock(ifElse->GetBody(), expected);
        std::shared_ptr<Node> elseNode = ifElse->GetElseNode();
        if (elseNode != nullptr)
        {
            assert((elseNode->GetType() == NodeType::IfElse || elseNode->GetType() == NodeType::Block) && "Unexpected else node body type");

            if (elseNode->GetType() == NodeType::IfElse) {
                AssumeIfElse(std::dynamic_pointer_cast<Statements::IfElseStatement>(elseNode), expected);
            } else {
                AssumeBlock(std::dynamic_pointer_cast<Statements::StatementsBody>(elseNode), expected);
            }
        }
    }

    void TypeChecker::AssumeWhile(std::shared_ptr<WhileNode> whileNode, PValType expected)
    {
        AssumeBlock(whileNode->GetBody(), expected);
    }

    void TypeChecker::AssumeForLoop(std::shared_ptr<ForLoopNode> forLoopNode, PValType expected)
    {
        AssumeBlock(forLoopNode->GetBody(), expected);
    }

    // Look for return statement inside block and check the type of returned value
    void TypeChecker::AssumeBlock(std::shared_ptr<Statements::StatementsBody> body, PValType expected)
    {
        auto it = std::make_shared<Common::Iterator>(body->GetSize());

        while (it->HasItems())
        {
            auto stmt = body->GetItem(it->GetPosition());
            it->Next();
            if (stmt->GetType() == NodeType::Return)
            {
                auto ret = std::dynamic_pointer_cast<ReturnNode>(stmt);
                PValType retType = DiagnoseNode(ret->GetValue());
                if (!retType->Compare(*expected))
                {
                    Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                    errorManager.UnexpectedReturnType(GetFilename(), expected, retType, ret->GetLocation());
                    ReportError();
                }
            }
            else if (stmt->GetType() == NodeType::IfElse)
            {
                AssumeIfElse(std::dynamic_pointer_cast<Statements::IfElseStatement>(stmt), expected);
            }
            else if (stmt->GetType() == NodeType::While)
            {
                AssumeWhile(std::dynamic_pointer_cast<WhileNode>(stmt), expected);
            }
            else
            {
                DiagnoseNode(stmt);
            }
        }
    }
}

