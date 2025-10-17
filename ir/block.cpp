#include <string>
#include <format>
#include "ir/block.h"
#include "ir/function.h"

namespace IR {

static std::string BlockToIndex(const std::map<const Block*, uint32_t>& block_to_index,
                                Block* block) {
    if (const auto it = block_to_index.find(block); it != block_to_index.end()) {
        return std::format("{{Block ${}}}", it->second);
    }
    return std::format("$<unknown block {:016x}>", reinterpret_cast<uint64_t>(block));
}

static uint32_t InstIndex(std::map<const Inst*, uint32_t>& inst_to_index, uint32_t& inst_index,
                        const Inst* inst) {
    const auto [it, is_inserted] = inst_to_index.emplace(inst, inst_index + 1);
    if (is_inserted) {
        ++inst_index;
    }
    return it->second;
}

static std::string ArgToIndex(std::map<const Inst*, uint32_t>& inst_to_index, uint32_t& inst_index,
                              const Value& arg) {
    if (arg.IsEmpty()) {
        return "<null>";
    }
    if (!arg.IsImmediate() || arg.IsIdentity()) {
        return std::format("%{}", InstIndex(inst_to_index, inst_index, arg.inst));
    }
    switch (arg.type) {
    case Type::I32:
        return std::format("#{}", arg.imm_i32);
    case Type::Function:
        return std::format("{}", arg.func->name);
    case Type::Reg:
        return std::format("R{}", arg.reg.index);
    default:
        return "<unknown immediate type>";
    }
}

std::string DumpBlock(const Block& block) {
    uint32_t inst_index{0};
    std::map<const Inst*, uint32_t> inst_to_index;
    return DumpBlock(block, {}, inst_to_index, inst_index);
}

std::string DumpBlock(const Block& block, const std::map<const Block*, uint32_t>& block_to_index,
                      std::map<const Inst*, uint32_t>& inst_to_index, uint32_t& inst_index) {
    std::string ret{"Block"};
    if (const auto it = block_to_index.find(&block); it != block_to_index.end()) {
        ret += std::format(" ${}", it->second);
    }
    ret += '\n';
    for (const Inst& inst : block.instructions) {
        const Opcode op = inst.GetOpcode();
        ret += std::format("[{:016x}] ", reinterpret_cast<uint64_t>(&inst));
        if (IsVoid(op)) {
            ret += std::format("         {}", NameOf(op)); // '%00000 = ' -> 1 + 5 + 3 = 9 spaces
        } else {
            ret += std::format("%{:<5} = {}", InstIndex(inst_to_index, inst_index, &inst), NameOf(op));
        }
        const auto arg_count = NumArgsOf(inst.GetOpcode());
        for (uint32_t arg_index = 0; arg_index < arg_count; ++arg_index) {
            const Value arg = inst.Arg(arg_index);
            const std::string arg_str = ArgToIndex(inst_to_index, inst_index, arg);
            ret += arg_index != 0 ? ", " : " ";
            ret += arg_str;
        }
        ret += '\n';
    }
    return ret;
}


} // namespace IR