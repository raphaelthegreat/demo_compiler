#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "backend/mix/mix_item.h"
#include "backend/mix/mix_opcode.h"
#include "ir/value.h"

namespace IR {
struct Program;
struct Function;
struct Block;
struct Inst;
}

namespace Mix {

using SymbolIndex = int;

struct Address {
    Short base;
    Index index;
    uint8_t field;
};

struct Location {
    enum class Type {
        RA,
        RJ,
        RX,
        Index,
        Zero,
    };
    Type type;
    Index index;
};

class Emitter {
    static constexpr uint32_t NumWords = 4000;
    static constexpr uint32_t NumIndexRegs = 6;
    static constexpr Short BaseRegAddr = Short{NumWords - 1U};

public:
    explicit Emitter(IR::Program& program);
    ~Emitter() = default;

private:
    void EmitBlock(const IR::Block& block);

    void LowerLoadReg(const IR::Inst& inst);
    void LowerStoreReg(const IR::Inst& inst);
    void LowerAdd(const IR::Inst& inst);

private:
    void EmitAdd(Location a, Address b);
    void EmitSub(Location a, Address b);
    void EmitCompare(Location a, Address b);
    void EmitEnt(Location dst, int32_t value);
    void EmitLoad(Location dst, Address address);
    void EmitStore(Address address, Location src);

    Address AddressForReg(IR::Reg reg) {
        const auto [it, is_new] = reg_to_index.try_emplace(reg.index);
        if (is_new) {
            it->second = last_used_index--;
        }
        const auto sym_index = it->second;
        if (sym_index != current_index) {
            const Location dst = {
                .type = Location::Type::Index,
                .index = Index::I1,
            };
            EmitEnt(dst, sym_index);
            current_index = sym_index;
        }
        return Address{
            .base = BaseRegAddr,
            .index = Index::I1,
            .field = 0x5,
        };
    }

private:
    IR::Program& program;
    std::array<Word, NumWords> memory;
    std::array<Short, NumIndexRegs> rI;
    struct Bounds {
        uint32_t start_word;
        uint32_t end_word;
    };
    std::unordered_map<const IR::Function*, Bounds> function_bounds;
    std::unordered_map<uint32_t, SymbolIndex> reg_to_index;
    SymbolIndex last_used_index{};
    SymbolIndex current_index{-1};
    uint32_t code_ptr{};
    Word rA, rX, rJ;
};

} // namespace Mix