#include <unordered_map>
#include "ir/program.h"

namespace IR {

static void EliminateInBlock(IR::Block& block) {
    struct RegisterInfo {
        IR::Value register_value;
        bool set_instruction_present = false;
        IR::Inst* last_set_instruction;
    };
    std::unordered_map<uint32_t, RegisterInfo> infos;
    for (auto& inst : block.instructions) {
        if (inst.GetOpcode() == IR::Opcode::StoreReg) {
            auto& info = infos[inst.Arg(0).Reg().index];
            if (info.set_instruction_present) {
                info.last_set_instruction->Invalidate();
            }
            info.register_value = inst.Arg(1);
            info.set_instruction_present = true;
            info.last_set_instruction = &inst;
            continue;
        }
        if (inst.GetOpcode() == IR::Opcode::LoadReg) {
            auto& info = infos[inst.Arg(0).Reg().index];
            if (info.register_value.IsEmpty()) {
                info = {};
                info.register_value = IR::Value{&inst};
            } else {
                inst.ToIdentity(info.register_value);
            }
        }
    }
}

void LoadStoreElimination(IR::Program& program) {
    for (auto& function : program.functions) {
        for (auto& block : function.blocks) {
            EliminateInBlock(block);
        }
    }
}


} // namespace IR