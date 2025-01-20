#pragma once

#ifndef OBJECT_MEMBER_H
#define OBJECT_MEMBER_H
#include"../node.h"

namespace AST
{
	namespace Nodes
	{
		class ObjectMember : public Node
		{
		private:
			std::shared_ptr<Node> m_member;
			std::shared_ptr<Node> m_obj;
		public:
			ObjectMember(std::shared_ptr<Node> obj) : m_member(nullptr), m_obj(obj) {}
			ObjectMember(): m_member(nullptr), m_obj(nullptr) {}
			~ObjectMember() {}
		public:
			inline NodeType GetType() const override { return NodeType::ObjectMember; }
		private:
			// TODO:
			std::string Format() const override { return "ObjectMember"; }
		public:
			void SetMember(std::shared_ptr<Node> member) { m_member = member; }
			void SetObject(std::shared_ptr<Node> obj) { m_obj = obj; }
		};
	}
}

#endif
