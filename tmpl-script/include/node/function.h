#pragma once
#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H
#include<vector>
#include<memory>
#include"../node.h"

namespace AST
{
	namespace Nodes
	{
		class FunctionCall : public Node
		{
		private:
			std::shared_ptr<Node> m_callee;
			std::vector<std::shared_ptr<Node>> m_args;
		private:
			std::string Format() const override;
			inline NodeType GetType() const override { return NodeType::FunctionCall; }
		public:
			FunctionCall(std::shared_ptr<Node> callee, std::vector<std::shared_ptr<Node>> args) : m_callee(callee), m_args(args) {}
			~FunctionCall() {}
		public:
			inline std::shared_ptr<Node> GetCallee() const { return m_callee; }
			inline std::vector<std::shared_ptr<Node>>* GetArgs() { return &m_args; }
		};
	}
}

#endif