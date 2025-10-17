#include "frontend/ast_node.h"
#include "frontend/translate_pass.h"

#include <algorithm>
#include <array>
#include <cstdlib>

TranslatePass::TranslatePass(ASTNode* root) {
    for (auto* node : *root->children) {
        func = FunctionFromAst(node);
        EmitBlock(node->body);
    }
}

IR::Block* TranslatePass::EmitBlock(ASTNode* node) {
    Ensure<NodeType::Block>(node);
    auto* block = BlockFromAst(node);
    ir.SetBlock(block);
    auto& children = *node->children;
    for (auto it = children.begin(); it != children.end(); ++it) {
        auto* child = *it;
        switch (child->node_type) {
        case NodeType::DeclBlock: {
            for (auto* decl : *child->children) {
                if (decl->initializer) {
                    ir.StoreReg(RegFromAstSymbol(decl), EmitExpr(decl->initializer));
                }
            }
            break;
        }
        case NodeType::Block: {
            child->payload = node->payload;
            EmitBlock(child);
            break;
        }
        case NodeType::UnaryOp:
            std::abort();
            break;
        case NodeType::BinaryOp: {
            if (child->binary_op != BinaryOp::Assign) {
                std::abort();
            }
            ir.StoreReg(RegFromAstSymbol(child->node_a), EmitExpr(child->node_b));
            break;
        }
        case NodeType::While: {
            auto* header_block = BlockFromAst(child);
            ir.SetBlock(header_block);
            auto cond = ir.Equal(EmitExpr(child->cond), ir.ConstI32(0));
            auto* body_block = EmitBlock(child->body);
            auto merge_it = std::next(it);
            IR::Block* merge_block = merge_it != children.end() ? BlockFromAst(*merge_it) : &func->blocks.emplace_back();
            auto* loop_block = &*func->blocks.emplace(--func->blocks.end());
            header_block->Branch(cond, merge_block);
            loop_block->Branch(ir.ConstI32(1), header_block);
            ir.SetBlock(merge_block);
            break;
        }
        case NodeType::Break: {
            auto* parent = child->parent;
            while (parent && parent->node_type != NodeType::While) {
                parent = parent->parent;
            }
            Ensure<NodeType::While>(parent);
            auto& top_children = *parent->parent->children;
            auto while_it = std::ranges::find(top_children, parent);
            auto merge_it = std::next(while_it);
            IR::Block* merge_block = merge_it != top_children.end() ? BlockFromAst(*merge_it) : &func->blocks.emplace_back();
            block->Branch(ir.ConstI32(1), merge_block);
            break;
        }
        case NodeType::IfElse: {
            auto cond = ir.Equal(EmitExpr(child->cond), ir.ConstI32(0));
            auto* if_block = EmitBlock(child->if_body);
            if (child->else_body) {
                auto* else_block = EmitBlock(child->else_body);
                block->Branch(cond, else_block);
            }
            auto merge_it = std::next(it);
            IR::Block* merge_block = merge_it != children.end() ? BlockFromAst(*merge_it) : &func->blocks.emplace_back();
            if (child->else_body) {
                if_block->Branch(ir.ConstI32(1), merge_block);
            }
            ir.SetBlock(merge_block);
            break;
        }
        case NodeType::Return: {
            if (child->ret_expr) {
                ir.Return(EmitExpr(child->ret_expr));
            } else {
                ir.Return();
            }
            break;
        }
        default:
            std::abort();
        }
    }
    return block;
}

IR::Value TranslatePass::EmitExpr(ASTNode* expr) {
    if (!expr) {
        std::abort();
    }
    if (expr->node_type == NodeType::ConstInt) {
        return ir.ConstI32(expr->const_val);
    }
    if (expr->node_type == NodeType::Symbol) {
        return ir.LoadReg(RegFromAstSymbol(expr));
    }
    if (expr->node_type == NodeType::Call) {
        auto& params = *expr->params->children;
        std::array<IR::Value, IR::MaxFuncArgs> args{};
        for (int i = 0; i < params.size(); ++i) {
            args[i] = EmitExpr(params[i]);
        }
        return ir.Call(FunctionFromAst(expr->func), args[0], args[1], args[2], args[3]);
    }
    Ensure<NodeType::BinaryOp>(expr);
    switch (expr->binary_op) {
    case BinaryOp::GreaterThan:
        return ir.GreaterThan(EmitExpr(expr->node_a), EmitExpr(expr->node_b));
    case BinaryOp::Add:
        return ir.Add(EmitExpr(expr->node_a), EmitExpr(expr->node_b));
    case BinaryOp::Subtract:
        return ir.Subtract(EmitExpr(expr->node_a), EmitExpr(expr->node_b));
    case BinaryOp::Multiply:
        return ir.Multiply(EmitExpr(expr->node_a), EmitExpr(expr->node_b));
    case BinaryOp::Divide:
        return ir.Divide(EmitExpr(expr->node_a), EmitExpr(expr->node_b));
    default:
        std::abort();
    }
    return IR::Value{};
}
