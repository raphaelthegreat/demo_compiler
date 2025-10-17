#include "backend/mix/mix_emitter.h"
#include "frontend/ast_node.h"
#include "ir/program.h"

namespace Mix {

using Type = Location::Type;

Emitter::Emitter(IR::Program& program_) : program{program_} {
    for (auto& function : program.functions) {
        auto& bounds = function_bounds[&function];
        bounds.start_word = code_ptr;
        for (auto& block : function.blocks) {
            EmitBlock(block);
        }
        bounds.end_word = code_ptr - 1;
    }
}

void Emitter::EmitBlock(const IR::Block& block) {
    for (auto& inst : block.instructions) {
        switch (inst.GetOpcode()) {
        case IR::Opcode::LoadReg:
            LowerLoadReg(inst);
            break;
        case IR::Opcode::StoreReg:
            LowerStoreReg(inst);
            break;
        case IR::Opcode::Add:
            LowerAdd(inst);
            break;
        }
    }
}

void Emitter::LowerLoadReg(const IR::Inst& inst) {
    const auto address = AddressForReg(inst.Arg(0).Reg());
}

void Emitter::LowerStoreReg(const IR::Inst& inst) {
    const auto address = AddressForReg(inst.Arg(0).Reg());
}

void Emitter::LowerAdd(const IR::Inst& inst) {
    const auto a = inst.Arg(0);
    const auto b = inst.Arg(1);
    if (b.IsImmediate()) {

    }
}

void Emitter::EmitAdd(Location a, Address b) {
    const auto opcode = a.type == Type::RA ? Opcode::ADD : (a.type == Type::Index ? Opcode::CMP1 + a.index : Opcode::CMPX);
    memory[code_ptr++] = Encode(b.base, b.index, b.field, opcode);
}

void Emitter::EmitSub(Location a, Address b) {
    const auto opcode = a.type == Type::RA ? Opcode::CMPA : (a.type == Type::Index ? Opcode::CMP1 + a.index : Opcode::CMPX);
    memory[code_ptr++] = Encode(b.base, b.index, b.field, opcode);

}

void Emitter::EmitEnt(Location dst, int32_t value) {
    const auto opcode = dst.type == Type::RA ? Opcode::ENTA : (dst.type == Type::Index ? Opcode::ENT1 + dst.index : Opcode::ENTX);
    memory[code_ptr++] = Encode({}, Index::None, 0x5, opcode);
}

void Emitter::EmitCompare(Location a, Address b) {
    const auto opcode = a.type == Type::RA ? Opcode::CMPA : (a.type == Type::Index ? Opcode::CMP1 + a.index : Opcode::CMPX);
    memory[code_ptr++] = Encode(b.base, b.index, b.field, opcode);
}

void Emitter::EmitLoad(Location dst, Address address) {
    const auto opcode = dst.type == Type::RA ? Opcode::LDA : (dst.type == Type::Index ? Opcode::LD1 + dst.index : Opcode::LDX);
    memory[code_ptr++] = Encode(address.base, address.index, address.field, opcode);
}

void Emitter::EmitStore(Address address, Location src) {
    using Type = Location::Type;
    const auto opcode = src.type == Type::RA ? Opcode::STA : (src.type == Type::Index ? Opcode::ST1 + src.index : (src.type == Type::Zero ? Opcode::STZ : Opcode::STX));
    memory[code_ptr++] = Encode(address.base, address.index, address.field, opcode);
}

} // namespace Mix