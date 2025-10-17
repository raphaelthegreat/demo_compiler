#pragma once

#include <cstdint>

namespace IR {

enum class Type {
    Void,
    Opaque,
    Function,
    Reg,
    I32,
};

struct Inst;
struct Function;

struct Reg {
    uint32_t index;
};

struct Value {
    explicit Value(Inst* inst_) : type{Type::Opaque}, inst{inst_} {}
    explicit Value(Function* func_) : type{Type::Function}, func{func_} {}
    explicit Value(int32_t imm) : type{Type::I32}, imm_i32{imm} {}
    explicit Value(Reg reg_) : type{Type::Reg}, reg{reg_} {}
    Value() : type{Type::Void} {}

    bool IsEmpty() const {
        return type == Type::Void;
    }

    bool IsImmediate() const {
        return type != Type::Opaque;
    }

    bool IsIdentity() const;

    Type type;
    union {
        Inst* inst{};
        Function* func;
        int32_t imm_i32;
        Reg reg;
    };
};

} // namespace IR