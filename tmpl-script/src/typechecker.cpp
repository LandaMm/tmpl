
#include <cassert>
#include <memory>
#include "../include/typechecker.h"
#include "../include/node/return.h"
#include "../include/node/identifier.h"
#include "../include/node/statement.h"
#include "../include/node/var_declaration.h"
#include "../include/node/function.h"
#include "../include/node/procedure.h"
#include "../include/node/unary.h"
#include "../include/interpreter/value.h"
#include "../include/node/logical.h"
#include "include/node/macros.h"

namespace Runtime
{
    using namespace AST::Nodes;

    ValueType TypeChecker::DiagnoseNode(std::shared_ptr<Node> node)
    {
        switch(node->GetType())
        {
            case NodeType::Expression:
                return DiagnoseExpression(std::dynamic_pointer_cast<ExpressionNode>(node));
            case NodeType::Literal:
                return DiagnoseLiteral(std::dynamic_pointer_cast<LiteralNode>(node));
            case NodeType::Identifier:
                return DiagnoseId(std::dynamic_pointer_cast<IdentifierNode>(node));
            case NodeType::Unary:
                return DiagnoseUnary(std::dynamic_pointer_cast<UnaryNode>(node));
            case NodeType::FunctionCall:
                return DiagnoseFnCall(std::dynamic_pointer_cast<FunctionCall>(node));
            case NodeType::VarDecl:
                HandleVarDeclaration(std::dynamic_pointer_cast<VarDeclaration>(node));
                return ValueType::Null;
            case NodeType::Condition:
                return DiagnoseCondition(std::dynamic_pointer_cast<Condition>(node));
            case NodeType::Ternary:
                return DiagnoseTernary(std::dynamic_pointer_cast<TernaryNode>(node));
            case NodeType::Return:
            {
                auto ret = std::dynamic_pointer_cast<ReturnNode>(node);
                return DiagnoseNode(ret->GetValue());
            }
            case NodeType::IfElse:
            {
                auto ifElse = std::dynamic_pointer_cast<Statements::IfElseStatement>(node);
                DiagnoseNode(ifElse->GetCondition());
                DiagnoseNode(ifElse->GetBody());
                if (ifElse->GetElseNode() != nullptr) DiagnoseNode(ifElse->GetElseNode());
                return ValueType::Null;
            }
            case NodeType::Block:
            {
                auto block = std::dynamic_pointer_cast<Statements::StatementsBody>(node);
                auto scope = std::make_shared<Environment<TypeVariable>>(m_variables);
                m_variables = scope;
                ValueType value = ValueType::Null;
                block->ResetIterator();
                while (block->HasItems())
                {
                    std::shared_ptr<Node> node = block->Next();
                    if (node->GetType() == NodeType::Return)
                    {
                        value = DiagnoseNode(node);
                        break;
                    }
                    else
                    {
                        auto localVal = DiagnoseNode(node);
                        if (node->IsBlock())
                        {
                            value = localVal;
                            break;
                        }
                    }
                }
                block->ResetIterator();
                assert(scope->GetParent() != nullptr && "Scope parent gone.");
                m_variables = scope->GetParent();
                assert(m_variables != scope && "Parent and child are located at the same memory space.");
                return value;
            }
            default:
                // just skip the nodes we cannot typecheck
                return ValueType::Null;
        }
    }

    void TypeChecker::RunChecker(std::shared_ptr<ProgramNode> program)
    {
        while(auto stmt = program->Next())
        {
            // procedures, macros and exports
            switch(stmt->GetType())
            {
                case NodeType::ProcedureDecl:
                {
                    auto proc = std::dynamic_pointer_cast<ProcedureDeclaration>(stmt);
                    DiagnoseNode(proc->GetBody());
                    break;
                }
                case NodeType::Require:
                    RunModuleChecker(std::dynamic_pointer_cast<RequireMacro>(stmt));
                    break;
                case NodeType::FnDecl:
                    HandleFnDeclaration(std::dynamic_pointer_cast<FunctionDeclaration>(stmt), false);
                    break;
                case NodeType::Extern:
                {
                    auto ext = std::dynamic_pointer_cast<ExternMacro>(stmt);
                    HandleFnSignature(ext->GetFnSignature(), false);
                    break;
                }
                default:
                    break;
            }
        }
        program->ResetIterator();
    }
 }
