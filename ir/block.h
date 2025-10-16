#pragma once

#include <list>
#include "ir/instruction.h"

namespace IR {

struct Block {
    std::list<IR::Inst> instructions;
};

} // namespace IR