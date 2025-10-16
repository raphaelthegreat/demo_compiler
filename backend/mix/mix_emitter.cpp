#include "backend/mix/mix_emitter.h"
#include "frontend/ast_node.h"

namespace Mix {

using Type = Location::Type;

Emitter::Emitter(const ASTNode* root) {

}

void Emitter::EmitExpr(Location dst, const ASTNode* expr) {
    if (expr->node_type == NodeType::ConstInt) {
        // location = const_val
    }
    if (expr->node_type == NodeType::Symbol) {
        // location = memory[reg]
    }
    if (expr->node_type == NodeType::Call) {
        auto* func = expr->func;
        auto& func_params = *expr->params->children;
        for (int i = 0; i < func_params.size(); ++i) {
            const auto* param = func_params[i];
            const bool is_last = i == func_params.size() - 1;
            //params += DumpExpr(param);
            if (!is_last) {
                //params += ", ";
            }
        }
        //return std::format("{}({})", func->name, params);
    }
    if (expr->node_type != NodeType::BinaryOp) {
        std::abort();
    }
    switch (expr->binary_op) {
    case BinaryOp::Assign:
        //EmitStore(GetSymbolAddress(expr->node_a), EmitExpr(expr->node_b));
        break;
    case BinaryOp::GreaterThan:
        //ret += std::format("({} > {})", EmitExpr(expr->node_a), EmitExpr(expr->node_b));
        break;
    case BinaryOp::Add:
        //ret += std::format("({} + {})", EmitExpr(expr->node_a), EmitExpr(expr->node_b));
        break;
    case BinaryOp::Subtract:
        //ret += std::format("({} - {})", EmitExpr(expr->node_a), EmitExpr(expr->node_b));
        break;
    case BinaryOp::Multiply:
        //ret += std::format("({} * {})", EmitExpr(expr->node_a), EmitExpr(expr->node_b));
        break;
    case BinaryOp::Divide:
        //ret += std::format("({} / {})", EmitExpr(expr->node_a), EmitExpr(expr->node_b));
        break;
    default:
        std::abort();
    }
}

void Emitter::EmitBlock(const ASTNode* node) {
    if (node->node_type != NodeType::Block) {
        std::abort();
    }
    for (auto* child : *node->children) {
        switch (child->node_type) {
        case NodeType::Function: {   // type, params, name, body
            std::string params;
            const auto& func_params = *child->params->children;
            for (int i = 0; i < func_params.size(); ++i) {
                const auto* param = func_params[i];
                const bool is_last = i == func_params.size() - 1;
                if (param->node_type != NodeType::Symbol) {
                    std::abort();
                }
                //params += std::format("int {}", param->name);
                if (!is_last) {
                    params += ", ";
                }
            }
            //ret += std::format("{}int {}({}) {{\n", indent, child->name, params);
            //ret += DumpBlock(indentation + 2, child->body);
            //ret += std::format("{}}}\n", indent);
            break;
        }
        case NodeType::Block:
            EmitBlock(child);
            break;
        case NodeType::UnaryOp:
            std::abort();
            break;
        case NodeType::BinaryOp:
            //EmitExpr(child);
            break;
        case NodeType::While:
            //ret += std::format("{}while({}) {{\n", indent, EmitExpr(child->cond));
            //ret += DumpBlock(indentation + 2, child->body);
            //ret += std::format("{}}}\n", indent);
            break;
        case NodeType::Break:
            //EmitBreak();
            break;
        case NodeType::IfElse:
            //ret += std::format("{}if({}) {{\n", indent, DumpExpr(child->cond));
            //ret += DumpBlock(indentation + 2, child->if_body);
            //ret += std::format("{}}} else {{\n", indent);
            //ret += DumpBlock(indentation + 2, child->else_body);
            //ret += std::format("{}}}\n", indent);
            break;
        case NodeType::Return: {
            //ret += std::format("{}return", indent);
            if (child->ret_expr) {
                auto* ret_expr = child->ret_expr;
                //ret += std::format(" {};\n", DumpExpr(ret_expr));
            } else {
                //ret += ";\n";
            }
            break;
        }
        case NodeType::Symbol: {
            const bool needs_store = child->initializer != nullptr;
            const auto address = GetSymbolAddress(child, needs_store);
            if (needs_store) {
                //EmitStore(address, EmitExpr(child->initializer));
            }
            break;
        }
        default:
            std::abort();
        }
    }
}

void Emitter::EmitAdd(Location a, Address b) {
    const auto opcode = a.type == Type::RA ? Opcode::ADD : (a.type == Type::Index ? Opcode::CMP1 + a.index : Opcode::CMPX);
    memory[code_ptr++] = Encode(b.base, b.index, b.field, opcode);

}

void Emitter::EmitSub(Location a, Address b) {
    const auto opcode = a.type == Type::RA ? Opcode::CMPA : (a.type == Type::Index ? Opcode::CMP1 + a.index : Opcode::CMPX);
    memory[code_ptr++] = Encode(b.base, b.index, b.field, opcode);

}

void Emitter::EmitEnt(Location dst, int32_t value) {
    
}

void Emitter::EmitCompare(Location a, Address b) {
    const auto opcode = a.type == Type::RA ? Opcode::CMPA : (a.type == Type::Index ? Opcode::CMP1 + a.index : Opcode::CMPX);
    memory[code_ptr++] = Encode(b.base, b.index, b.field, opcode);
}

void Emitter::EmitLoad(Location dst, Address address) {
    const auto opcode = dst.type == Type::RA ? Opcode::LDA : (dst.type == Type::Index ? Opcode::LD1 + dst.index : Opcode::LDX);
    memory[code_ptr++] = Encode(address.base, address.index, address.field, opcode);
}

void Emitter::EmitStore(Address address, Location src) {
    using Type = Location::Type;
    const auto opcode = src.type == Type::RA ? Opcode::STA : (src.type == Type::Index ? Opcode::LD1 + src.index : Opcode::LDX);
    memory[code_ptr++] = Encode(address.base, address.index, address.field, opcode);
}

} // namespace Mix