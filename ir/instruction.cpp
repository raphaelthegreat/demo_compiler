#include "ir/instruction.h"

namespace IR {

std::string_view NameOf(Opcode op) {
    switch (op) {
    case Opcode::Void: return "Void";
    case Opcode::Add: return "Add";
    case Opcode::Subtract: return "Subtract";
    case Opcode::Multiply: return "Multiply";
    case Opcode::Divide: return "Divide";
    case Opcode::LessThan: return "LessThan";
	case Opcode::LessThanEqual: return "LessThanEqual";
	case Opcode::GreaterThan: return "GreaterThan";
	case Opcode::GreaterThanEqual: return "GreaterThanEqual";
	case Opcode::Equal: return "Equal";
	case Opcode::NotEqual: return "NotEqual";
    case Opcode::LoadReg: return "LoadReg";
    case Opcode::StoreReg: return "StoreReg";
    case Opcode::Call: return "Call";
    case Opcode::Return: return "Return";
    case Opcode::Goto: return "Goto";
    case Opcode::Identity: return "Identity";
    }
    return "Unknown";
}

bool IsVoid(Opcode op) {
    return op == Opcode::Void || op == Opcode::StoreReg || op == Opcode::Return || op == Opcode::Goto;
}

uint32_t NumArgsOf(Opcode op) {
    switch (op) {
    case Opcode::Add:
    case Opcode::Subtract:
    case Opcode::Multiply:
    case Opcode::Divide:
    case Opcode::LessThan:
	case Opcode::LessThanEqual:
	case Opcode::GreaterThan:
	case Opcode::GreaterThanEqual:
	case Opcode::Equal:
	case Opcode::NotEqual:
    case Opcode::StoreReg:
    case Opcode::Goto:
        return 2;
    case Opcode::LoadReg:
    case Opcode::Return:
    case Opcode::Identity:
        return 1;
    case Opcode::Call:
        return 5;
    default:
        return 0;
    }
}

} // namespace IR