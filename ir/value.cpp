#include "ir/value.h"
#include "ir/instruction.h"

namespace IR {

bool Value::IsIdentity() const {
    return type == Type::Opaque && inst->GetOpcode() == Opcode::Identity;
}

} // namespace IR 