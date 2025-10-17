#pragma once

#include <array>
#include <cstring>
#include <string_view>
#include "ir/value.h"

namespace IR {

enum class Opcode {
    Void,
    Add,
    Subtract,
    Multiply,
    Divide,
    LessThan,
	LessThanEqual,
	GreaterThan,
	GreaterThanEqual,
	Equal,
	NotEqual,
    LoadReg,
    StoreReg,
    Call,
    Return,
    Goto,
    Identity,
};

std::string_view NameOf(Opcode op);

bool IsVoid(Opcode op);

uint32_t NumArgsOf(Opcode op);

struct Inst {
    static constexpr int32_t MAX_ARGS = 5;
    explicit Inst(Opcode opcode_) : opcode(opcode_) {}

    Opcode GetOpcode() const {
        return opcode;
    }

    Value Arg(uint32_t index) const {
        return args[index];
    }

    void SetArg(uint32_t index, Value arg) {
        args[index] = arg;
    }

    void Invalidate() {
        std::memset(&args, 0, sizeof(args));
        opcode = Opcode::Void;
    }

    void ToIdentity(Value val) {
        Invalidate();
        args[0] = val;
        opcode = Opcode::Identity;
    }

    void ReplaceOp(Opcode new_op) {
        opcode = new_op;
    }

    template <typename T>
    void SetDefinition(T def) {
        definition = std::bit_cast<uint32_t>(def);
    }

    template <typename T>
    T Definition() const noexcept {
        return std::bit_cast<T>(definition);
    }

private:
    Opcode opcode;
    std::array<Value, MAX_ARGS> args;
    uint32_t definition;
};

} // namespace IR