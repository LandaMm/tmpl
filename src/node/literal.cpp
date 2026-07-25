
#include<string>
#include"../../include/node/literal.h"

namespace AST
{
	namespace Nodes
	{
		std::string LiteralNode::Format() const
		{
			std::string formatted = "Literal(";

			switch (m_type)
			{
			case LiteralType::_NULL:
				formatted += "NULL";
				break;
			case LiteralType::BOOL:
				formatted += "BOOL";
				break;
			case LiteralType::DOUBLE:
				formatted += "DOUBLE";
				break;
			case LiteralType::FLOAT:
				formatted += "FLOAT";
				break;
			case LiteralType::INT:
				formatted += "INT";
				break;
			case LiteralType::STRING:
				formatted += "STRING";
				break;
			default:
				formatted += "UnknownType";
				break;
			}

			formatted += ")";
			return formatted;
		}
	}
}