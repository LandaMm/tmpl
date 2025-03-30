
#include "include/ir.h"
#include "include/ir/block.h"
#include "include/ir/phi.h"
#include "include/ir/ternary.h"

namespace IR
{
    std::string JmpInst::Format() const
    {
        return "jmp " + GetJmpLabel()->Format();
    }

    std::string BranchInst::Format() const
    {
        return "br " + GetTarget()->Format() + ", " + GetTrueBLabel()->Format() + ", " + GetFalseBLabel()->Format();
    }

    shared_ptr<IRTemp> IRParser::ParseTernary(shared_ptr<TernaryNode> tern)
    {
        auto condVal = ParseNodeWithValue(tern->GetCondition());

        auto trueBlock = std::make_shared<BlockInst>(GetBID());
        auto falseBlock = std::make_shared<BlockInst>(GetBID());

        auto brInst = std::make_shared<BranchInst>(condVal, trueBlock->GetLabel(), falseBlock->GetLabel());
        AddInst(brInst);

        auto mergeBlock = std::make_shared<BlockInst>(GetBID());

        auto currWriter = ReplaceWriter(trueBlock);
        auto left = ParseNodeWithValue(tern->GetLeft());
        AddInst(std::make_shared<JmpInst>(mergeBlock->GetLabel()));
        SetWriter(currWriter);

        currWriter = ReplaceWriter(falseBlock);
        auto right = ParseNodeWithValue(tern->GetRight());
        AddInst(std::make_shared<JmpInst>(mergeBlock->GetLabel()));
        SetWriter(currWriter);

        AddInst(trueBlock);
        AddInst(falseBlock);

        auto phi = std::make_shared<PhiInst>(GetTID(), left, right);
        mergeBlock->WriteInst(phi);

        AddInst(mergeBlock);

        SetWriter(mergeBlock);

        return phi;
    }
}

