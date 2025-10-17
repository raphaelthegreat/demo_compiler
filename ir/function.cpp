#include <map>
#include <print>
#include "ir/function.h"

namespace IR {

void DumpFunction(const Function& func) {
    uint32_t index{};
    std::map<const IR::Inst*, uint32_t> inst_to_index;
    std::map<const IR::Block*, uint32_t> block_to_index;

    std::print("Function {}\n", func.name);

    for (const IR::Block& block : func.blocks) {
        block_to_index.emplace(&block, index);
        ++index;
    }

    for (const auto& block : func.blocks) {
        std::string s = IR::DumpBlock(block, block_to_index, inst_to_index, index);
        std::print("{}\n", s);
    }
}

} // namespace IR