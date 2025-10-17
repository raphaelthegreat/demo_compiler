#include <cstdlib>
#include "ir/value.h"
#include "ir/instruction.h"

namespace IR {

bool Value::IsIdentity() const {
    return type == Type::Opaque && inst->GetOpcode() == Opcode::Identity;
}

IR::Inst* Value::InstRecursive() const {
    if (IsIdentity()) {
        return inst->Arg(0).InstRecursive();
    }
    if (type != Type::Opaque) {
        std::abort();
    }
    return inst;
}

} // namespace IR 