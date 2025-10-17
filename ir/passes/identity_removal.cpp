#include "ir/program.h"
#include <vector>

namespace IR {

void IdentityRemovalPass(IR::Program& program) {
    std::vector<IR::Inst*> to_invalidate;
    for (auto& function : program.functions) {
        for (auto& block : function.blocks) {
            for (auto inst = block.instructions.begin(); inst != block.instructions.end();) {
                const size_t num_args = NumArgsOf(inst->GetOpcode());
                for (size_t i = 0; i < num_args; ++i) {
                    IR::Value arg;
                    while ((arg = inst->Arg(i)).IsIdentity()) {
                        inst->SetArg(i, arg.Inst()->Arg(0));
                    }
                }
                if (inst->GetOpcode() == IR::Opcode::Identity ||
                    inst->GetOpcode() == IR::Opcode::Void) {
                    to_invalidate.push_back(&*inst);
                    inst = block.instructions.erase(inst);
                } else {
                    ++inst;
                }
            }
        }
    }
    for (IR::Inst* const inst : to_invalidate) {
        inst->Invalidate();
    }
}

} // namespace IR