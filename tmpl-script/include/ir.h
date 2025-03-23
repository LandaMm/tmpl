
#ifndef IR_H_
#define IR_H_

#include "include/node.h"
#include "include/node/expression.h"
#include "include/node/literal.h"
#include "include/node/program.h"
#include "include/node/statement.h"
#include "include/parser.h"
#include<memory>
#include <string>

namespace IR
{
    using std::shared_ptr;
    using namespace AST::Nodes;

    enum class InstType {
        Binary, // Math & Logic

        Cmp,
        Br,
        Jmp, // (Goto)
        Phi,

        Load,
        Store,
        Alloca,
        LoadConst,

        Call,
        Ret,
        Param,

        BasicBlock,
    };

    typedef unsigned long tid;

    class IRTemp
    {
    private:
        tid m_id;
    public:
        IRTemp(tid id) : m_id(id) { }
    public:
        inline tid GetId() const { return m_id; }
    public:
        inline std::string Format() const { return "%" + std::to_string(GetId()); }
    };

    class IRInst
    {
    private:
        InstType m_type;
    public:
        IRInst(InstType typ) : m_type(typ) { }
    public:
        inline InstType GetType() const { return m_type; }
    public:
        virtual std::string Format() const
            { return "IRInst (" + std::to_string((int)m_type) + ")"; }
    };

    class IRParser
    {
    private:
        shared_ptr<ProgramNode> m_program;
        std::string m_filename;
    private:
        std::vector<shared_ptr<IRInst>> m_parsed;
        tid m_temp_id = 0;
    public:
        IRParser(shared_ptr<AST::Parser> parser)
            : m_program(std::dynamic_pointer_cast<ProgramNode>(parser->GetRoot())),
              m_filename(parser->GetFilename()) { }
    public:
        void Parse();
    private:
        void ParseNode(shared_ptr<AST::Node> node);
        shared_ptr<IRTemp> ParseNodeWithValue(shared_ptr<AST::Node> node);

        void ParseBlock(shared_ptr<AST::Statements::StatementsBody> body);

        shared_ptr<IRTemp> ParseExpression(shared_ptr<ExpressionNode> expr);
        shared_ptr<IRTemp> ParseLiteral(shared_ptr<LiteralNode> lit);
    private:
        void AddInst(shared_ptr<IRInst> inst) { m_parsed.push_back(inst); }
        tid GetTID() { return m_temp_id++; }
    public:
        inline unsigned int GetSize() const { return m_parsed.size(); }
        inline shared_ptr<IRInst> GetInst(unsigned int index) const { return m_parsed[index]; }
    };
}

#endif // IR_H_

