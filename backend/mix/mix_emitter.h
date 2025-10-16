#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "backend/mix/mix_item.h"
#include "backend/mix/mix_opcode.h"

struct ASTNode;

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
    };
    Type type;
    Index index;
};

class Emitter {
    static constexpr uint32_t NUM_WORDS = 4000;
    static constexpr uint32_t NUM_INDEX_REGS = 6;
    static constexpr Short BASE_REG_ADDR = Short{NUM_WORDS - 1U};

public:
    explicit Emitter(const ASTNode* root);
    ~Emitter() = default;

private:
    void EmitBlock(const ASTNode* node);
    void EmitExpr(Location dst, const ASTNode* expr);

    void EmitAdd(Location a, Address b);
    void EmitSub(Location a, Address b);
    void EmitCompare(Location a, Address b);
    void EmitEnt(Location dst, int32_t value);
    void EmitLoad(Location dst, Address address);
    void EmitStore(Address address, Location src);

    Address GetSymbolAddress(const ASTNode* sym, bool prepare_index = true) {
        const auto [it, is_new] = symbol_to_index.try_emplace(sym);
        if (is_new) {
            it->second = last_used_index--;
        }
        const auto sym_index = it->second;
        if (sym_index != current_index && prepare_index) {
            const Location dst = {
                .type = Location::Type::Index,
                .index = Index::I1,
            };
            EmitEnt(dst, sym_index);
            current_index = sym_index;
        }
        return Address{
            .base = BASE_REG_ADDR,
            .index = Index::I1,
            .field = 0x5,
        };
    }

private:
    std::array<Word, NUM_WORDS> memory;
    std::array<Short, NUM_INDEX_REGS> rI;
    std::unordered_map<const ASTNode*, SymbolIndex> symbol_to_index;
    SymbolIndex last_used_index{};
    SymbolIndex current_index{-1};
    int code_ptr{};
    Word rA, rX, rJ;
};

} // namespace Mix