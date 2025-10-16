#pragma once

#include <cstdint>

namespace IR {

enum class Type {
    Opaque,
    I32,
};

struct Inst;

struct Value {
    Type type;
    union {
        IR::Inst* inst;
        int32_t imm_i32;
    };
};

} // namespace IR