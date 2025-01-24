#pragma once
#ifndef NODE_H
#define NODE_H
#include<iostream>
#include<string>
#include<vector>
#include<memory>

namespace AST
{
	namespace Nodes { }

	enum class NodeType
	{
		Program,
		Expression,
		FunctionCall,
		Literal,
		Identifier,
		ObjectMember,
		List,
		Condition,
		Ternary,
		Unary,
		IfElse,
		Block,
		VarDecl,
	};

	class Node
	{
	public:
		virtual inline NodeType GetType() const = 0;
	private:
		virtual std::string Format() const = 0;
	public:
		template<typename T>
		inline T* Get() const { return static_cast<T*>(this); };

	public:
		friend std::ostream& operator<<(std::ostream& stream, const Node& node);
	};

	// 2 + var - ((10 * 5) / 2) * 4.4
	// "string" + "string"

	
}

#endif
