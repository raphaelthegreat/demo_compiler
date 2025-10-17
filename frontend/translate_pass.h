#pragma once

#include <unordered_map>
#include "ir/ir_emitter.h"
#include "ir/program.h"

struct ASTNode;

class TranslatePass {
public:
    TranslatePass(ASTNode* root);

    IR::Program& Program() {
        return program;
    }

private:
    IR::Block* EmitBlock(ASTNode* block);
    IR::Value EmitExpr(ASTNode* expr);

    IR::Reg RegFromAstSymbol(ASTNode* sym) {
        Ensure<NodeType::Symbol>(sym);
        if (sym->payload == NoPayload) {
            sym->payload = current_reg_index++;
        }
        return IR::Reg{uint32_t(sym->payload)};
    }

    IR::Block* BlockFromAst(ASTNode* node) {
        // Ensure<NodeType::Block>(node);
        if (node->payload == NoPayload) {
            node->SetPayload<IR::Block*>(&func->blocks.emplace_back());
        }
        return node->Payload<IR::Block*>();
    }

    IR::Function* FunctionFromAst(ASTNode* node) {
        Ensure<NodeType::Function>(node);
        if (node->payload == NoPayload) {
            node->SetPayload<IR::Function*>(&program.functions.emplace_back(node->name));
        }
        return node->Payload<IR::Function*>();
    }

    template <NodeType type>
    void Ensure(const ASTNode* node) {
        if (!node || node->node_type != type) {
            std::abort();
        }
    }

private:
    IR::IREmitter ir;
    IR::Program program;
    IR::Function* func;
    uint32_t current_reg_index{};
};