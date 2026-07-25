
#ifndef PROCEDURE_H
#define PROCEDURE_H
#include <memory>
#include <string>
#include "statement.h"

namespace AST
{
    namespace Nodes
    {
        class ProcedureDeclaration : public Node
        {
        private:
            std::string m_name;
            std::shared_ptr<Statements::StatementsBody> m_body;

        public:
            std::string Format() const override;
            inline NodeType GetType() const override { return NodeType::ProcedureDecl; }

        public:
            ProcedureDeclaration(std::string name, std::shared_ptr<Statements::StatementsBody> body, Location loc)
                : m_name(name), m_body(body), Node(loc) {}
            ~ProcedureDeclaration() {}

        public:
            inline std::string GetName() const { return m_name; }
            inline std::shared_ptr<Statements::StatementsBody> GetBody() const { return m_body; }
        };
    }
}

#endif
