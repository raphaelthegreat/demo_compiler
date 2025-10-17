#pragma once

namespace IR {

struct Program;

void LoadStoreElimination(IR::Program& program);
void IdentityRemovalPass(IR::Program& program);

} // namespace IR