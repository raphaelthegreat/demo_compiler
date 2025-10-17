#pragma once

#include <cstdint>

namespace IR {

enum class Type {
    Void,
    Opaque,
    Function,
    Block,
    Reg,
    I32,
};

struct Inst;
struct Function;
struct Block;

struct Reg {
    uint32_t index;
};

struct Value {
    explicit Value(IR::Inst* inst_) : type{Type::Opaque}, inst{inst_} {}
    explicit Value(Function* func_) : type{Type::Function}, func{func_} {}
    explicit Value(Block* block_) : type{Type::Block}, block{block_} {}
    explicit Value(int32_t imm) : type{Type::I32}, imm_i32{imm} {}
    explicit Value(IR::Reg reg_) : type{Type::Reg}, reg{reg_} {}
    Value() : type{Type::Void} {}

    IR::Reg Reg() const {
        return reg;
    }

    IR::Inst* Inst() const {
        return inst;
    }

    Function* Func() const {
        return func;
    }

    bool IsEmpty() const {
        return type == Type::Void;
    }

    bool IsImmediate() const {
        return type != Type::Opaque;
    }

    bool IsIdentity() const;
    IR::Inst* InstRecursive() const;

    Type type;
    union {
        IR::Inst* inst{};
        Function* func;
        Block* block;
        int32_t imm_i32;
        IR::Reg reg;
    };
};

} // namespace IR