#pragma once

#include <list>
#include <map>
#include <string>
#include "ir/instruction.h"

namespace IR {

struct Block {
    Block() = default;
    ~Block() = default;

    void Link(Value cond_, Block* true_branch_, Block* false_branch_) {
        cond = cond_;
        true_branch = true_branch_;
        false_branch_ = false_branch_;
    }

    std::list<Inst> instructions;
    Block* true_branch;
    Block* false_branch;
    Value cond;
};

std::string DumpBlock(const Block& block);

std::string DumpBlock(const Block& block, const std::map<const Block*, uint32_t>& block_to_index,
                      std::map<const Inst*, uint32_t>& inst_to_index, uint32_t& inst_index);

} // namespace IR