
#ifndef IR_H_
#define IR_H_

#include "include/node.h"
#include "include/node/expression.h"
#include "include/node/function.h"
#include "include/node/identifier.h"
#include "include/node/literal.h"
#include "include/node/logical.h"
#include "include/node/program.h"
#include "include/node/statement.h"
#include "include/node/unary.h"
#include "include/parser.h"
#include<memory>
#include <string>
#include <unordered_map>

namespace IR
{
    using std::shared_ptr;
    using namespace AST::Nodes;

    enum class InstType {
        Binary, // Math & Logic
        Neg,
        Not,

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

    enum class DataType
    {
        // Void
        Void,
        // Bool
        I1,
        // Int
        I8,
        I16,
        I32,
        I64,
        I128,
        // Float
        F32,
        F64,
    };

    inline std::string FormatDataType(DataType typ)
    {
        switch(typ)
        {
            case DataType::Void:
                return "void";
            case DataType::I1:
                return "i1";
            case DataType::I8:
                return "i8";
            case DataType::I16:
                return "i16";
            case DataType::I32:
                return "i32";
            case DataType::I64:
                return "i64";
            case DataType::I128:
                return "i128";
            case DataType::F32:
                return "f32";
            case DataType::F64:
                return "f64";
            default:
                break;
        }

        assert(false && "Should be able to handle all data types for format");

        return "";
    }

    typedef unsigned long tid; // temp id
    typedef unsigned long bid; // block id

    class IRTemp
    {
    private:
        tid m_id;
        DataType m_dataType;
    public:
        IRTemp(tid id, DataType dataType) : m_id(id), m_dataType(dataType) { }
    public:
        inline tid GetId() const { return m_id; }
        inline DataType GetDataType() const { return m_dataType; }
        void SetDataType(DataType dataTyp) { m_dataType = dataTyp; }
    public:
        inline std::string Format() const { return "%" + std::to_string(GetId()); }
    };

    class IRLabel
    {
    private:
        bid m_id;
    public:
        IRLabel(bid id) : m_id(id) { }
    public:
        inline bid GetId() const { return m_id; }
    public:
        inline std::string Format() const { return "%L" + std::to_string(GetId()); }
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

    class IRWriter
    {
    public:
        IRWriter() { }
    public:
        virtual void WriteInst(shared_ptr<IRInst> inst) = 0;
    };

    class IRParser : public IRWriter
    {
    private:
        shared_ptr<ProgramNode> m_program;
        std::string m_filename;
        std::unordered_map<std::string, DataType> m_fn_dataType;
        std::unordered_map<std::string, DataType> m_var_dataType;
    private:
        std::vector<shared_ptr<IRInst>> m_parsed;

        shared_ptr<IRWriter> m_current_writer;

        tid m_temp_id = 0;
        bid m_block_id = 0;
    public:
        IRParser(shared_ptr<AST::Parser> parser)
            : m_program(std::dynamic_pointer_cast<ProgramNode>(parser->GetRoot())),
              m_filename(parser->GetFilename()),
              m_current_writer(nullptr) { }
    public:
        void Parse();
    private:
        void ParseNode(shared_ptr<AST::Node> node);
        shared_ptr<IRTemp> ParseNodeWithValue(shared_ptr<AST::Node> node);

        void ParseBlock(shared_ptr<AST::Statements::StatementsBody> body);

        shared_ptr<IRTemp> ParseExpression(shared_ptr<ExpressionNode> expr);
        shared_ptr<IRTemp> ParseLiteral(shared_ptr<LiteralNode> lit);
        shared_ptr<IRTemp> ParseIdentifier(shared_ptr<IdentifierNode> id);
        shared_ptr<IRTemp> ParseFunctionCall(shared_ptr<FunctionCall> fnCall);
        shared_ptr<IRTemp> ParseCondition(shared_ptr<Condition> cond);
        shared_ptr<IRTemp> ParseTernary(shared_ptr<TernaryNode> tern);
        shared_ptr<IRTemp> ParseUnary(shared_ptr<UnaryNode> unary);
    private:
        shared_ptr<IRWriter> ReplaceWriter(shared_ptr<IRWriter> writer)
        {
            auto saved = m_current_writer;
            m_current_writer = writer;
            return saved;
        }
        void SetWriter(shared_ptr<IRWriter> writer)
        {
            m_current_writer = writer;
        }

        virtual void WriteInst(shared_ptr<IRInst> inst) override { m_parsed.push_back(inst); }
        void AddInst(shared_ptr<IRInst> inst) { m_current_writer->WriteInst(inst); }

        tid GetTID() { return m_temp_id++; }
        shared_ptr<IRLabel> GetBID() { return std::make_shared<IRLabel>(m_block_id++); }
    public:
        inline unsigned int GetSize() const { return m_parsed.size(); }
        inline shared_ptr<IRInst> GetInst(unsigned int index) const { return m_parsed[index]; }
    public: // Functions
        void RegisterFnReturnType(std::string fnName, DataType dataTyp);
        DataType* GetFnReturnType(std::string fnName);
    public: // Variables
        void RegisterVarType(std::string varName, DataType dataTyp);
        DataType* GetVarReturnType(std::string varName);
    };
}

#endif // IR_H_

