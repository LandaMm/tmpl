#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <string>
#include "cpl-parser/location.h"

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
        Return, //
        FnDecl, // 
        Require, // 
        Export, // 
        Extern, //
        Type,
        TypeDf,
        Instance,
        Cast,
        Assign,
        While,
        Break,
        For,
    };

	class Node
	{
	public:
        Node(LocationSpan loc) : m_loc(loc) { }
		virtual ~Node() = default;

		Node(const Node&) = delete;
		Node& operator=(const Node&) = delete;

		Node(Node&&) = delete;
		Node& operator=(Node&&) = delete;
	public:
		template <typename T>
		inline T *Get() const { return static_cast<T *>(this); };

		template<typename T>
		requires std::derived_from<T, Node>
		inline T* As() { return dynamic_cast<T*>(this); }
    public:
        inline LocationSpan GetLocation() const { return m_loc; }
    public:
        void SetLocation(LocationSpan loc) { m_loc = loc; }
    public:
		virtual inline NodeType GetType() const = 0;
        virtual inline bool IsBlock() { return false; }
    private:
        LocationSpan m_loc;
	};

}

#endif
