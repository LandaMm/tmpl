
#include "include/ir.h"
#include "include/ir/block.h"
#include "include/iterator.h"
#include "include/node.h"
#include "include/node/expression.h"
#include "include/node/function.h"
#include "include/node/identifier.h"
#include "include/node/literal.h"
#include "include/node/logical.h"
#include "include/node/procedure.h"
#include <memory>
#include <utility>

namespace IR
{
    void IRParser::RegisterFnReturnType(std::string fnName, DataType dataTyp)
    {
        m_fn_dataType.insert(std::make_pair(fnName, dataTyp));
    }

    DataType* IRParser::GetFnReturnType(std::string fnName)
    {
        auto item = m_fn_dataType.find(fnName);
        if (item == m_fn_dataType.end())
        {
            return nullptr;
        }
        return &item->second;
    }

    void IRParser::RegisterVarType(std::string varName, DataType dataTyp)
    {
        m_var_dataType.insert(std::make_pair(varName, dataTyp));
    }

    DataType* IRParser::GetVarReturnType(std::string varName)
    {
        auto item = m_var_dataType.find(varName);
        if (item == m_var_dataType.end())
        {
            return nullptr;
        }
        return &item->second;
    }

    void IRParser::Parse()
    {
        auto entry = std::make_shared<BlockInst>(GetBID());
        SetWriter(entry);
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
        WriteInst(entry);
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
            case AST::NodeType::Condition:
            case AST::NodeType::Identifier:
            case AST::NodeType::FunctionCall:
            case AST::NodeType::Ternary:
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
            case AST::NodeType::Condition:
                return ParseCondition(std::dynamic_pointer_cast<Condition>(node));
            case AST::NodeType::FunctionCall:
                return ParseFunctionCall(std::dynamic_pointer_cast<FunctionCall>(node));
            case AST::NodeType::Identifier:
                return ParseIdentifier(std::dynamic_pointer_cast<IdentifierNode>(node));
            case AST::NodeType::Ternary:
                return ParseTernary(std::dynamic_pointer_cast<TernaryNode>(node));
            default:
                std::cerr << "[ParseNodeWithValue] Unsupported node: "
                    << node->Format() << std::endl;
                std::exit(1);
        }

        assert(false && "Should be unreachable");
        return nullptr;
    }
}

