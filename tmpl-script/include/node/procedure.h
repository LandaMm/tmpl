
#ifndef PROCEDURE_H
#define PROCEDURE_H
#include <memory>
#include <string>
#include "statement.h"

namespace AST
{
    namespace Nodes
    {
        class ProcedureDeclaration : public Statements::StatementsNode
        {
        private:
            std::string m_name;

        private:
            std::string Format() const override;
            inline NodeType GetType() const override { return NodeType::ProcedureDecl; }

        public:
            ProcedureDeclaration(std::string name)
                : m_name(name),
                  StatementsNode() {}
            ~ProcedureDeclaration() {}

        public:
            inline std::string GetName() const { return m_name; }
        };
    }
}

#endif
