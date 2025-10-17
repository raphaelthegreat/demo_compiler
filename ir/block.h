#pragma once

#include <list>
#include <map>
#include <string>
#include "ir/instruction.h"

namespace IR {

struct Block {
    Block() = default;
    ~Block() = default;

    void Branch(Value cond, Block* branch) {
        auto& inst = instructions.emplace_back(Opcode::Goto);
        inst.SetArg(0, cond);
        inst.SetArg(1, Value{branch});
    }

    std::list<Inst> instructions;
};

std::string DumpBlock(const Block& block);

std::string DumpBlock(const Block& block, const std::map<const Block*, uint32_t>& block_to_index,
                      std::map<const Inst*, uint32_t>& inst_to_index, uint32_t& inst_index);

} // namespace IR