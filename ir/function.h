#pragma once

#include <list>
#include <string_view>
#include "ir/block.h"

namespace IR {

struct Function {
    Function(std::string_view name_) : name{name_} {}

    std::string_view name;
    int num_args;
    std::list<Block> blocks;
};

void DumpFunction(const Function& func);

} // namespace IR