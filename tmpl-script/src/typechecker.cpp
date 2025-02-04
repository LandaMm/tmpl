
#include <cassert>
#include <memory>
#include "../include/typechecker.h"
#include "../include/error.h"
#include "../include/node/return.h"
#include "../include/node/identifier.h"
#include "../include/node/statement.h"
#include "../include/node/var_declaration.h"
#include "../include/node/function.h"
#include "include/node/procedure.h"

namespace Runtime
{
    using namespace AST::Nodes;

    ValueType TypeChecker::DiagnoseLiteral(std::shared_ptr<LiteralNode> literal)
    {
        switch(literal->GetLiteralType())
        {
            case LiteralType::INT:
                return ValueType::Integer;
            case LiteralType::DOUBLE:
                return ValueType::Double;
            case LiteralType::FLOAT:
                return ValueType::Float;
            case LiteralType::STRING:
                return ValueType::String;
            case LiteralType::BOOL:
                return ValueType::Bool;
            default:
                break;
        }

        assert(literal->GetLiteralType() != LiteralType::_NULL &&
                "Literal type shouldn't be left null");
        assert(false && "Unreachable. Should handle all literal types");

        return ValueType::Null;
    }


    ValueType TypeChecker::DiagnoseExpression(std::shared_ptr<ExpressionNode> expr)
    {
        auto left = DiagnoseNode(expr->GetLeft());
        auto right = DiagnoseNode(expr->GetRight());

        if (left != right)
        {
            Prelude::ErrorManager& manager = Prelude::ErrorManager::getInstance();
            manager.OperandMismatchType(GetFilename(), left, right, expr->GetLocation(), "TypeError");
            return ValueType::Null;
        }

        return left;
    }

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
            case NodeType::FunctionCall:
                return DiagnoseFnCall(std::dynamic_pointer_cast<FunctionCall>(node));
            case NodeType::VarDecl:
                HandleVarDeclaration(std::dynamic_pointer_cast<VarDeclaration>(node));
                return ValueType::Null;
            case NodeType::Block:
            {
                auto block = std::dynamic_pointer_cast<Statements::StatementsBody>(node);
                while (auto stmt = block->Next())
                {
                    DiagnoseNode(stmt);
                }
                block->ResetIterator();
                return ValueType::Null;
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
                case NodeType::FnDecl:
                    HandleFnDeclaration(std::dynamic_pointer_cast<FunctionDeclaration>(stmt));
                    break;
                // TODO: add require, export support
                default:
                    break;
            }
        }
        program->ResetIterator();
    }

    ValueType TypeChecker::DiagnoseId(std::shared_ptr<IdentifierNode> identifier)
    {
		if (!m_variables->HasItem(identifier->GetName()))
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.UndeclaredVariable(GetFilename(), identifier, "TypeError");
			return ValueType::Null;
		}
		std::shared_ptr<TypeVariable> var = m_variables->LookUp(identifier->GetName());
		return var->GetType();
    }

    ValueType TypeChecker::DiagnoseFnCall(std::shared_ptr<FunctionCall> fnCall)
    {
        // TODO: support for object member calls
        assert(fnCall->GetCallee()->GetType() == NodeType::Identifier
                && "Unimplemented fn call callee node type");

        std::shared_ptr<IdentifierNode> callee =
            std::dynamic_pointer_cast<IdentifierNode>(fnCall->GetCallee());

        std::string fnName = callee->GetName();

        if (!m_functions->HasItem(fnName))
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.UndeclaredFunction(GetFilename(), callee, "TypeError");
            return ValueType::Null;
        }

        std::shared_ptr<TypeFn> fn = m_functions->LookUp(fnName);

        auto args = fnCall->GetArgs();

        if (fn->GetParamsSize() != args->size())
        {
            Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
            errorManager.ArgsParamsExhausted(
                    GetFilename(),
                    fnName,
                    args->size(),
                    fn->GetParamsSize(),
                    fnCall->GetLocation(), "TypeError");
            return ValueType::Null;
        }

        while (fn->HasParams())
        {
            std::shared_ptr<Node> arg = (*args)[fn->GetParamsIndex()];
            std::shared_ptr<FnParam> param = fn->GetNextParam();
            ValueType valType = DiagnoseNode(arg);
            if (valType != param->GetType())
            {
                Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                errorManager.ArgMismatchType(
                        GetFilename(),
                        param->GetName(),
                        valType,
                        param->GetType(),
                        arg->GetLocation(),
                        "TypeError"
                        );
                return ValueType::Null;
            }
        }

        return fn->GetReturnType();
    }

    void TypeChecker::AssumeIfElse(std::shared_ptr<Statements::IfElseStatement> ifElse, ValueType expected)
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

    // Look for return statement inside block and check the type of returned value
    void TypeChecker::AssumeBlock(std::shared_ptr<Statements::StatementsBody> body, ValueType expected)
    {
        body->ResetIterator();

        while (auto stmt = body->Next())
        {
            if (stmt->GetType() == NodeType::Return)
            {
                auto ret = std::dynamic_pointer_cast<ReturnNode>(stmt);
                ValueType retType = DiagnoseNode(ret->GetValue());
                if (retType != expected)
                {
                    Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
                    errorManager.UnexpectedReturnType(GetFilename(), expected, retType, ret->GetLocation());
                }
            }
            else if (stmt->GetType() == NodeType::IfElse)
            {
                AssumeIfElse(std::dynamic_pointer_cast<Statements::IfElseStatement>(stmt), expected);
            }
        }

        body->ResetIterator();
    }
    
    void TypeChecker::HandleFnDeclaration(std::shared_ptr<FunctionDeclaration> fnDecl)
    {
        std::shared_ptr<Statements::StatementsBody> body = fnDecl->GetBody();
        ValueType retType = EvaluateType(GetFilename(), fnDecl->GetReturnType());

        std::string name = fnDecl->GetName();

        std::shared_ptr<TypeFn> fn = std::make_shared<TypeFn>(retType);

        auto currentScope = m_variables;
        auto variables = std::make_shared<Environment<TypeVariable>>(m_variables);

        while (fnDecl->HasParams())
        {
            std::shared_ptr<FunctionParam> param = fnDecl->GetNextParam();
            ValueType paramType = EvaluateType(GetFilename(), param->GetType());
            std::string paramName = param->GetName()->GetName();
            std::shared_ptr<FnParam> fnParam = std::make_shared<FnParam>(paramType, paramName);
            fn->AddParam(fnParam);
            auto typeVar = std::make_shared<TypeVariable>(paramType, false);
            variables->AddItem(paramName, typeVar);
        }
        fnDecl->ResetIterator();

        m_variables = variables;

        AssumeBlock(body, retType);

        m_variables = currentScope;

        m_functions->AddItem(name, fn);
    }

    void TypeChecker::HandleVarDeclaration(std::shared_ptr<VarDeclaration> varDecl)
    {
		ValueType varType = EvaluateType(GetFilename(), varDecl->GetType());
		std::string varName = *varDecl->GetName();
		ValueType varValueType = DiagnoseNode(varDecl->GetValue());

		if (varType != varValueType)
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.VarMismatchType(GetFilename(), varName, varValueType, varType, varDecl->GetLocation(), "TypeError");
			return;
		}

		std::shared_ptr<TypeVariable> var = std::make_shared<TypeVariable>(varType, varDecl->Editable());

        if (m_variables->HasItem(varName))
        {
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.VarAlreadyExists(GetFilename(), varName, varDecl->GetLocation(), "TypeError");
			return;
        }

		m_variables->AddItem(varName, var);
    }

    ValueType TypeChecker::EvaluateType(std::string filename, std::shared_ptr<Node> typeNode)
    {
		if (typeNode->GetType() == NodeType::Identifier)
		{
			auto id = std::dynamic_pointer_cast<IdentifierNode>(typeNode);
			if (id->GetName() == "string")
			{
				return ValueType::String;
			}
			else if (id->GetName() == "double")
			{
				return ValueType::Double;
			}
			else if (id->GetName() == "float")
			{
				return ValueType::Float;
			}
			else if (id->GetName() == "int")
			{
				return ValueType::Integer;
			}
			else
			{
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.UndefinedType(filename, id->GetName(), id->GetLocation(), "TypeError");
			}
		}
		else
		{
			// TODO: support for complex types, e.g. inline objects, generic types
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported node type for type: " + std::to_string((int)typeNode->GetType()));
		}

		return ValueType::Null;
    }
}
