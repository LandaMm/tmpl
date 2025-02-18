#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <string>
#include "./location.h"

namespace AST
{
	namespace Nodes
	{
	}

	enum class NodeType
	{
		Program, //
		Expression, //
		FunctionCall, //
		Literal, // 
		Identifier, //
		ObjectMember,
		List,
		Condition, // 
		Ternary, // 
		Unary, // 
		IfElse, //
		Block, //
		VarDecl, // 
		ProcedureDecl, //
        Return, //
        FnDecl, // 
        Require, // 
        Export, // 
        Extern, //
        Type,
        TypeTemplate,
        TypeDf,
    };

	class Node
	{
    private:
        Location m_loc;
	public:
		virtual inline NodeType GetType() const = 0;

	public:
		virtual std::string Format() const = 0;

	public:
		template <typename T>
		inline T *Get() const { return static_cast<T *>(this); };

	public:
		friend std::ostream &operator<<(std::ostream &stream, const Node &node);

    public:
        inline Location GetLocation() const { return m_loc; }

    public:
        void SetLocation(Location loc) { m_loc = loc; }

    public:
        virtual inline bool IsBlock() { return false; }

    public:
        // Node() : m_loc(Location(-1, -1)) { }
        Node(Location loc) : m_loc(loc) { }
	};

	// 2 + var - ((10 * 5) / 2) * 4.4
	// "string" + "string"

}

#endif
