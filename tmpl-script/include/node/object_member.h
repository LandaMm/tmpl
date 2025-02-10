
#ifndef OBJECT_MEMBER_H
#define OBJECT_MEMBER_H
#include<memory>
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
			ObjectMember(std::shared_ptr<Node> obj, Location loc)
                : m_member(nullptr), m_obj(obj), Node(loc) {}
			ObjectMember(Location loc)
                : m_member(nullptr), m_obj(nullptr), Node(loc) {}
			~ObjectMember() {}
		public:
			inline NodeType GetType() const override { return NodeType::ObjectMember; }
		public:
			std::string Format() const override;
		public:
			void SetMember(std::shared_ptr<Node> member) { m_member = member; }
			void SetObject(std::shared_ptr<Node> obj) { m_obj = obj; }
        public:
            inline std::shared_ptr<Node> GetMember() const { return m_member; }
            inline std::shared_ptr<Node> GetObject() const { return m_obj; }
		};
	}
}

#endif
