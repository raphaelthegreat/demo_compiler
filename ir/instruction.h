#pragma once

#include <array>
#include "ir/value.h"

namespace IR {

enum class Opcode {
    Add,
    Subtract,
    Multiply,
    Divide,
    ConditionRef,
    Identity,
    
};

struct Inst {
    static constexpr int32_t MAX_ARGS = 2;

    Opcode opcode;
    std::array<Value, MAX_ARGS> args;
};

} // namespace IR