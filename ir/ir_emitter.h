#pragma once

#include "ir/block.h"
#include "ir/value.h"
#include "ir/instruction.h"

namespace IR {

constexpr uint32_t MaxFuncArgs = 4;

class IREmitter {
public:
    explicit IREmitter() = default;
    ~IREmitter() = default;

    void SetBlock(Block* new_block) {
        block = new_block;
    }

    Value ConstI32(int32_t val);

    Value Add(Value a, Value b);
    Value Subtract(Value a, Value b);
    Value Multiply(Value a, Value b);
    Value Divide(Value a, Value b);

    Value LessThan(Value a, Value b);
	Value LessThanEqual(Value a, Value b);
	Value GreaterThan(Value a, Value b);
	Value GreaterThanEqual(Value a, Value b);
	Value Equal(Value a, Value b);
	Value NotEqual(Value a, Value b);

    Value Call(Function* func, Value a = {}, Value b = {}, Value c = {}, Value d = {});
    void Return(Value val = {});

    Value LoadReg(Reg reg);
    void StoreReg(Reg reg, Value value);

private:
    template <typename... Args>
    Value Inst(Opcode op, Args... args) {
        auto& inst = block->instructions.emplace_back(op);
        uint32_t index = 0;
        (inst.SetArg(index++, Value{args}), ...);
        return Value{&inst};
    }

private:
    Block* block;
};

} // namespace IR 