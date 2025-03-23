
#include "include/ir.h"
#include "include/iterator.h"
#include "include/node.h"
#include "include/node/expression.h"
#include "include/node/literal.h"
#include "include/node/procedure.h"
#include <memory>

namespace IR
{
    void IRParser::Parse()
    {
        for (auto it = Common::Iterator(m_program->Size()); it.HasItems(); it.Next())
        {
            auto stmt = m_program->GetItem(it.GetPosition());
            switch(stmt->GetType())
            {
                case AST::NodeType::ProcedureDecl:
                {
                    // TODO: separately parse procedure decls
                    auto proc = std::dynamic_pointer_cast<ProcedureDeclaration>(stmt);
                    ParseBlock(proc->GetBody());
                    break;
                }
                default:
                    std::cerr << "Unsupported node while compiling: "
                        << stmt->Format() << std::endl;
                    std::exit(1);
            }
        }
    }

    void IRParser::ParseBlock(shared_ptr<AST::Statements::StatementsBody> body)
    {
        for (auto it = Common::Iterator(body->GetSize()); it.HasItems(); it.Next())
        {
            auto stmt = body->GetItem(it.GetPosition());
            ParseNode(stmt);
        }
    }

    void IRParser::ParseNode(shared_ptr<AST::Node> node)
    {
        switch(node->GetType())
        {
            case AST::NodeType::Literal:
            case AST::NodeType::Expression:
                ParseNodeWithValue(node);
                break;
            default:
                std::cerr << "[ParseNode] Unsupported node while compiling: "
                    << node->Format() << std::endl;
                std::exit(1);
        }
    }

    shared_ptr<IRTemp> IRParser::ParseNodeWithValue(shared_ptr<AST::Node> node)
    {
        switch(node->GetType())
        {
            case AST::NodeType::Literal:
                return ParseLiteral(std::dynamic_pointer_cast<LiteralNode>(node));
            case AST::NodeType::Expression:
                return ParseExpression(std::dynamic_pointer_cast<ExpressionNode>(node));
            default:
                std::cerr << "[ParseNodeWithValue] Unsupported node: "
                    << node->Format() << std::endl;
                std::exit(1);
        }

        assert(false && "Should be unreachable");
        return nullptr;
    }
}

