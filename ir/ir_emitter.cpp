#include "ir/block.h"
#include "ir/ir_emitter.h"

namespace IR {

Value IREmitter::ConstI32(int32_t val) {
    return Value{val};
}

Value IREmitter::Add(Value a, Value b) {
    return Inst(Opcode::Add, a, b);
}

Value IREmitter::Subtract(Value a, Value b) {
    return Inst(Opcode::Subtract, a, b);
}

Value IREmitter::Multiply(Value a, Value b) {
    return Inst(Opcode::Multiply, a, b);
}

Value IREmitter::Divide(Value a, Value b) {
    return Inst(Opcode::Divide, a, b);
}

Value IREmitter::LessThan(Value a, Value b) {
    return Inst(Opcode::LessThan, a, b);
}

Value IREmitter::LessThanEqual(Value a, Value b) {
    return Inst(Opcode::LessThanEqual, a, b);
}

Value IREmitter::GreaterThan(Value a, Value b) {
    return Inst(Opcode::GreaterThan, a, b);
}

Value IREmitter::GreaterThanEqual(Value a, Value b) {
    return Inst(Opcode::GreaterThanEqual, a, b);
}

Value IREmitter::Equal(Value a, Value b) {
    return Inst(Opcode::Equal, a, b);
}

Value IREmitter::NotEqual(Value a, Value b) {
    return Inst(Opcode::NotEqual, a, b);
}

Value IREmitter::Call(Function* func, Value a, Value b, Value c, Value d) {
    return Inst(Opcode::Call, func, a, b, c, d);
}

void IREmitter::Return(Value val) {
    Inst(Opcode::Return, val);
}

void IREmitter::Goto(Value cond, Block* target) {
    Inst(Opcode::Goto, cond, target);
}

Value IREmitter::LoadReg(Reg reg) {
    return Inst(Opcode::LoadReg, reg);
}

void IREmitter::StoreReg(Reg reg, Value value) {
    Inst(Opcode::StoreReg, reg, value);
}

} // namespace IR