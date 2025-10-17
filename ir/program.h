#pragma once

#include <deque>
#include "ir/function.h"

namespace IR {

struct Program {
    std::deque<Function> functions;
};

} // namespace IR