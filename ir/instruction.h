#pragma once

#include <array>
#include <string_view>
#include "ir/value.h"

namespace IR {

enum class Opcode {
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

private:
    Opcode opcode;
    std::array<Value, MAX_ARGS> args;
};

} // namespace IR