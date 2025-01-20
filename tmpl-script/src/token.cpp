
#include<iostream>
#include"../include/token.h"

namespace AST
{
	std::ostream& operator<<(std::ostream& stream, const Token& token)
	{
		stream << "Token(";

		switch (token.GetType())
		{
			case TokenType::Id:
				stream << "Id, " << *token.GetValue<std::string>();
				break;
			case TokenType::Integer:
				stream << "Integer, " << *token.GetValue<int>();
				break;
			case TokenType::Float:
				stream << "Float, " << *token.GetValue<float>();
				break;
			case TokenType::Double:
				stream << "Double, " << *token.GetValue<double>();
				break;
			case TokenType::String:
				stream << "String, " << *token.GetValue<std::string>();
				break;
			case TokenType::Whitespace:
				stream << "Whitespace";
				break;
			case TokenType::OpenBracket:
				stream << "OpenBracket";
				break;
			case TokenType::CloseBracket:
				stream << "CloseBracket";
				break;
			case TokenType::OpenCurly:
				stream << "OpenCurly";
				break;
			case TokenType::CloseCurly:
				stream << "CloseCurly";
				break;
			case TokenType::OpenSquareBracket:
				stream << "OpenSquareBracket";
				break;
			case TokenType::CloseSquareBracket:
				stream << "CloseSquareBracket";
				break;
			case TokenType::Newline:
				stream << "Newline";
				break;
			case TokenType::Point:
				stream << "Point";
				break;
			case TokenType::Plus:
				stream << "Plus";
				break;
			case TokenType::Minus:
				stream << "Minus";
				break;
			case TokenType::Multiply:
				stream << "Multiply";
				break;
			case TokenType::Divide:
				stream << "Divide";
				break;
			case TokenType::_EOF:
				stream << "EOF";
				break;
			default:
				stream << "`UNKNOWN`";
				break;
		}

		stream << ")";

		return stream;
	}
}
