#include "../include/interpreter.h"
#include "../include/error.h"

namespace Runtime
{
	using namespace AST::Nodes;

	std::shared_ptr<Value> Interpreter::EvaluateTernary(std::shared_ptr<TernaryNode> ternary)
	{
		std::shared_ptr<Value> condition = Evaluate(ternary->GetCondition());

		if (condition->GetType() != ValueType::Integer)
		{
			// should be unreachable
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("RuntimeError: Condition cannot produce non-integer value");
			return nullptr;
		}

		std::shared_ptr<IntegerValue> cVal = std::dynamic_pointer_cast<IntegerValue>(condition);
		if (*cVal->GetValue() == 1)
		{
			return Evaluate(ternary->GetLeft());
		}

		return Evaluate(ternary->GetRight());
	}

	std::shared_ptr<Value> Interpreter::EvaluateCondition(std::shared_ptr<Condition> condition)
	{
		std::shared_ptr<Value> left = Evaluate(condition->GetLeft());
		std::shared_ptr<Value> right = Evaluate(condition->GetRight());

		if (left->GetType() != right->GetType() && left->GetType() != ValueType::Null && right->GetType() != ValueType::Null)
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.OperandMismatchType(left->GetType(), right->GetType());
			return nullptr;
		}

		if (left->GetType() != right->GetType() && (left->GetType() == ValueType::Null || right->GetType() == ValueType::Null))
		{
			if (condition->GetOperator() != Condition::ConditionType::Compare && condition->GetOperator() != Condition::ConditionType::NotEqual)
			{
				Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
				errorManager.RaiseError("Unsupported condition operator for null values: " + std::to_string((int)condition->GetOperator()));
				return nullptr;
			}
			if (condition->GetOperator() == Condition::ConditionType::Compare)
				return std::make_shared<IntegerValue>(std::make_shared<int>(0));
			else
				return std::make_shared<IntegerValue>(std::make_shared<int>(1));
		}

		return left->Compare(right, condition->GetOperator());
	}

	std::shared_ptr<Value> Interpreter::EvaluateIdentifier(std::shared_ptr<IdentifierNode> identifier)
	{
		if (!m_variables->HasItem(identifier->GetName()))
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.UndeclaredVariable(identifier);
			return nullptr;
		}
		std::shared_ptr<Variable> var = m_variables->LookUp(identifier->GetName());
		return var->GetValue();
	}

	ValueType Interpreter::EvaluateType(std::shared_ptr<Node> typeNode)
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
				errorManager.UndefinedType(id->GetName());
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

	void Interpreter::EvaluateProcedureDeclaration(std::shared_ptr<ProcedureDeclaration> procDecl)
	{
		std::string name = procDecl->GetName();
		std::shared_ptr<Statements::StatementsBody> body = procDecl->GetBody();

		std::shared_ptr<Procedure> proc = std::make_shared<Procedure>(body);
		m_procedures->AddItem(name, proc);
	}

	void Interpreter::EvaluateVariableDeclaration(std::shared_ptr<VarDeclaration> varDecl)
	{
		ValueType varType = EvaluateType(varDecl->GetType());
		std::string varName = *varDecl->GetName();
		std::shared_ptr<Value> varValue = Evaluate(varDecl->GetValue());

		if (varType != varValue->GetType())
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.VarMismatchType(varName, varValue->GetType(), varType);
			return;
		}

		std::shared_ptr<Variable> var = std::make_shared<Variable>(varType, varValue, varDecl->Editable());
		m_variables->AddItem(varName, var);
	}

	std::shared_ptr<Value> Interpreter::EvaluateLiteral(std::shared_ptr<LiteralNode> literal)
	{
		switch (literal->GetLiteralType())
		{
		case LiteralType::INT:
			return std::make_shared<IntegerValue>(*literal->GetValue<int>());
		case LiteralType::FLOAT:
			return std::make_shared<FloatValue>(*literal->GetValue<float>());
		case LiteralType::DOUBLE:
			return std::make_shared<DoubleValue>(*literal->GetValue<double>());
		case LiteralType::STRING:
			return std::make_shared<StringValue>(*literal->GetValue<std::string>());
		default:
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported literal type: " + std::to_string((int)literal->GetLiteralType()));
			break;
		}

		// TODO: return a NullValue?
		return nullptr;
	}

	std::shared_ptr<Value> Interpreter::EvaluateExpression(std::shared_ptr<ExpressionNode> expr)
	{
		std::shared_ptr<Value> left = Evaluate(expr->GetLeft());
		std::shared_ptr<Value> right = Evaluate(expr->GetRight());

		ValueType valueType = left->GetType();
		if (valueType != right->GetType())
		{
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported different types of operands: " + std::to_string((int)left->GetType()) + " != " + std::to_string((int)right->GetType()));
			return nullptr;
		}

		return left->Operate(right, expr->GetOperator().GetType());
	}

	std::shared_ptr<Value> Interpreter::Evaluate(std::shared_ptr<Node> node)
	{
		switch (node->GetType())
		{
		case NodeType::Expression:
			return EvaluateExpression(std::dynamic_pointer_cast<ExpressionNode>(node));
		case NodeType::Literal:
			return EvaluateLiteral(std::dynamic_pointer_cast<LiteralNode>(node));
			/*case NodeType::Program:
				std::shared_ptr<ProgramNode> program = std::dynamic_pointer_cast<ProgramNode>(node);
				while (auto stmt = program->Next())
				{

				}*/
		case NodeType::VarDecl:
			EvaluateVariableDeclaration(std::dynamic_pointer_cast<VarDeclaration>(node));
			return nullptr;
		case NodeType::ProcedureDecl:
			EvaluateProcedureDeclaration(std::dynamic_pointer_cast<ProcedureDeclaration>(node));
			return nullptr;
		case NodeType::Identifier:
			return EvaluateIdentifier(std::dynamic_pointer_cast<IdentifierNode>(node));
		case NodeType::Condition:
			return EvaluateCondition(std::dynamic_pointer_cast<Condition>(node));
		case NodeType::Ternary:
			return EvaluateTernary(std::dynamic_pointer_cast<TernaryNode>(node));
		default:
			Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported node type by evaluating: " + std::to_string((int)node->GetType()));
			break;
		}

		return nullptr;
	}
}