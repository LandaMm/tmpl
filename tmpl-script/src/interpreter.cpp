#include"../include/interpreter.h"
#include"../include/error.h"

namespace Runtime
{
	using namespace AST::Nodes;

	std::shared_ptr<Value> Interpreter::EvaluateIdentifier(std::shared_ptr<IdentifierNode> identifier)
	{
		if (!m_env->HasVariable(identifier->GetName()))
		{
			Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
			errorManager.UndeclaredVariable(identifier);
			return nullptr;
		}
		std::shared_ptr<Variable> var = m_env->LookUp(identifier->GetName());
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
			else {
				Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
				errorManager.UndefinedType(id->GetName());
			}
		}
		else
		{
			// TODO: support for complex types, e.g. inline objects, generic types
			Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported node type for type: " + std::to_string((int)typeNode->GetType()));
		}

		return ValueType::Null;
	}

	void Interpreter::EvaluateVariableDeclaration(std::shared_ptr<VarDeclaration> varDecl)
	{
		ValueType varType = EvaluateType(varDecl->GetType());
		std::string varName = *varDecl->GetName();
		std::shared_ptr<Value> varValue = Evaluate(varDecl->GetValue());

		if (varType != varValue->GetType())
		{
			Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
			errorManager.VarMismatchType(varName, varValue->GetType(), varType);
			return;
		}

		std::shared_ptr<Variable> var = std::make_shared<Variable>(varType, varValue, varDecl->Editable());
		m_env->AddVariable(varName, var);
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
			Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
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
			Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported different types of operands: " + std::to_string((int)left->GetType()) + " != " + std::to_string((int)right->GetType()));
			return nullptr;
		}

		switch (valueType)
		{
		case ValueType::Integer:
		{
			std::shared_ptr<IntegerValue> livv = std::dynamic_pointer_cast<IntegerValue>(left);
			std::shared_ptr<int> liv = livv->GetValue();
			std::shared_ptr<IntegerValue> rivv = std::dynamic_pointer_cast<IntegerValue>(right);
			std::shared_ptr<int> riv = rivv->GetValue();
			switch (expr->GetOperator().GetType())
			{
			case ExpressionNode::OperatorType::PLUS:
				return std::make_shared<IntegerValue>(std::make_shared<int>((*(liv)) + (*(riv))));
			case ExpressionNode::OperatorType::MINUS:
				return std::make_shared<IntegerValue>(std::make_shared<int>((*(liv)) - (*(riv))));
			case ExpressionNode::OperatorType::DIVIDE:
				return std::make_shared<IntegerValue>(std::make_shared<int>((*(liv)) / (*(riv))));
			case ExpressionNode::OperatorType::MULTIPLY:
				return std::make_shared<IntegerValue>(std::make_shared<int>((*(liv)) * (*(riv))));
			}
		}
		case ValueType::Float:
		{
			std::shared_ptr<FloatValue> livv = std::dynamic_pointer_cast<FloatValue>(left);
			std::shared_ptr<float> liv = livv->GetValue();
			std::shared_ptr<FloatValue> rivv = std::dynamic_pointer_cast<FloatValue>(right);
			std::shared_ptr<float> riv = rivv->GetValue();
			switch (expr->GetOperator().GetType())
			{
			case ExpressionNode::OperatorType::PLUS:
				return std::make_shared<FloatValue>(std::make_shared<float>((*(liv)) + (*(riv))));
			case ExpressionNode::OperatorType::MINUS:
				return std::make_shared<FloatValue>(std::make_shared<float>((*(liv)) - (*(riv))));
			case ExpressionNode::OperatorType::DIVIDE:
				return std::make_shared<FloatValue>(std::make_shared<float>((*(liv)) / (*(riv))));
			case ExpressionNode::OperatorType::MULTIPLY:
				return std::make_shared<FloatValue>(std::make_shared<float>((*(liv)) * (*(riv))));
			}
		}
		case ValueType::Double:
		{
			std::shared_ptr<DoubleValue> livv = std::dynamic_pointer_cast<DoubleValue>(left);
			std::shared_ptr<double> liv = livv->GetValue();
			std::shared_ptr<DoubleValue> rivv = std::dynamic_pointer_cast<DoubleValue>(right);
			std::shared_ptr<double> riv = rivv->GetValue();
			switch (expr->GetOperator().GetType())
			{
			case ExpressionNode::OperatorType::PLUS:
				return std::make_shared<DoubleValue>(std::make_shared<double>((*(liv)) + (*(riv))));
			case ExpressionNode::OperatorType::MINUS:
				return std::make_shared<DoubleValue>(std::make_shared<double>((*(liv)) - (*(riv))));
			case ExpressionNode::OperatorType::DIVIDE:
				return std::make_shared<DoubleValue>(std::make_shared<double>((*(liv)) / (*(riv))));
			case ExpressionNode::OperatorType::MULTIPLY:
				return std::make_shared<DoubleValue>(std::make_shared<double>((*(liv)) * (*(riv))));
			}
		}
		case ValueType::String:
		{
			std::shared_ptr<StringValue> livv = std::dynamic_pointer_cast<StringValue>(left);
			std::shared_ptr<std::string> liv = livv->GetValue();
			std::shared_ptr<StringValue> rivv = std::dynamic_pointer_cast<StringValue>(right);
			std::shared_ptr<std::string> riv = rivv->GetValue();
			if (expr->GetOperator().GetType() != ExpressionNode::OperatorType::PLUS)
			{
				Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
				errorManager.RaiseError("Unsupported operator for string literals: " + std::to_string((int)expr->GetOperator().GetType()));
				return nullptr;
			}
			return std::make_shared<StringValue>(std::make_shared<std::string>(*(liv)+*(riv)));
		}
		default:
			Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported value type: " + std::to_string((int)valueType));
			break;
		}

		return nullptr;
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
				std::shared_ptr<Nodes::ProgramNode> program = std::dynamic_pointer_cast<Nodes::ProgramNode>(node);
				while (auto stmt = program->Next())
				{

				}*/
		case NodeType::VarDecl:
			EvaluateVariableDeclaration(std::dynamic_pointer_cast<VarDeclaration>(node));
			return nullptr;
		case NodeType::Identifier:
			return EvaluateIdentifier(std::dynamic_pointer_cast<IdentifierNode>(node));
		default:
			Prelude::ErrorManager& errorManager = Prelude::ErrorManager::getInstance();
			errorManager.RaiseError("Unsupported node type by evaluating: " + std::to_string((int)node->GetType()));
			break;
		}

		return nullptr;
	}
}