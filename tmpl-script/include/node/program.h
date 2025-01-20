#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H
#include<string>
#include<vector>
#include<memory>
#include"../node.h"

namespace AST
{
	namespace Nodes
	{
		class ProgramNode : public Node
		{
		private:
			std::vector<std::shared_ptr<Node>> m_statements;
			size_t m_index;
		private:
			std::string Format() const override;
		public:
			ProgramNode(): m_index(-1), m_statements(std::vector<std::shared_ptr<Node>>()) {}
			~ProgramNode() {}
		public:
			inline NodeType GetType() const override { return NodeType::Program; }
		public:
			void AddStatement(std::shared_ptr<Node> statement);
		public:
			inline size_t Size() { return m_statements.size(); }
			std::shared_ptr<Node> Next();
			void Reset();
		};
	}
}

#endif
