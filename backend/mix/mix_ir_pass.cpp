#include "ir/program.h"

namespace Mix {

static void FoldInverseCompare(IR::Inst& inst) {
    auto* parent = inst.Arg(0).InstRecursive();
    bool replaced = false;
    switch (parent->GetOpcode()) {
        case IR::Opcode::LessThan:
            parent->ReplaceOp(IR::Opcode::GreaterThanEqual);
            replaced = true;
            break;
        case IR::Opcode::LessThanEqual:
            parent->ReplaceOp(IR::Opcode::GreaterThan);
            replaced = true;
            break;
        case IR::Opcode::GreaterThan:
            parent->ReplaceOp(IR::Opcode::LessThanEqual);
            replaced = true;
            break;
        case IR::Opcode::GreaterThanEqual:
            parent->ReplaceOp(IR::Opcode::LessThan);
            replaced = true;
            break;
        case IR::Opcode::Equal:
            parent->ReplaceOp(IR::Opcode::NotEqual);
            replaced = true;
            break;
        case IR::Opcode::NotEqual:
            parent->ReplaceOp(IR::Opcode::Equal);
            replaced = true;
            break;
        default:
            break;
    }
    if (replaced) {
        inst.ToIdentity(IR::Value{parent});
    }
}

static void PreprocessInst(IR::Inst& inst) {
    if (inst.GetOpcode() == IR::Opcode::Equal && inst.Arg(1).IsImmediate() && inst.Arg(1).imm_i32 == 0) {
        FoldInverseCompare(inst);
        return;
    }
    
}

void MixPreprocessPass(IR::Program& program) {
    for (auto& function : program.functions) {
        for (auto& block : function.blocks) {
            for (auto& inst : block.instructions) {
                PreprocessInst(inst);
            }
        }
    }
}

} // namespace Mix