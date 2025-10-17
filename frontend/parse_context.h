#pragma once

#include <array>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast_node.h"

struct SourceLoc {
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};

class ParseContext {
    using SymbolTable = std::unordered_map<std::string, ASTNode*>;

public:
    explicit ParseContext() {
        Push(); // Global namespace
    }

    void Push() {
        sym_tables.emplace_back();
    }

    void Pop() {
        sym_tables.pop_back();
    }

    ASTNode* Root() {
        return root;
    }

    void SetRoot(ASTNode* block) {
        root = block;
    }

    void SetDeclSeqType(ASTNode* type) {
        decl_seq_type = type;
    }

    ASTNode* DeclSeqType() const {
        return decl_seq_type;
    }

    void PrintTree();

    ASTNode* FindSymbol(const std::string& name);

    ASTNode* CreateIntType();
    ASTNode* CreateIntConst(int val);
    ASTNode* CreateSymbol(ASTNode* type, const std::string& name,
                          ASTNode* initializer = nullptr);
    ASTNode* CreateFunction(const SourceLoc& loc, ASTNode* return_type,
                            const std::string& name, ASTNode* params,
                            ASTNode* body);
    ASTNode* CreateDeclBlock(ASTNode* initial_child);
    ASTNode* CreateBlock(ASTNode* initial_child);
    ASTNode* CreateBinaryOp(BinaryOp op, ASTNode* node_a, ASTNode* node_b);
    ASTNode* CreateIfElse(ASTNode* cond, ASTNode* if_body, ASTNode* else_body);
    ASTNode* CreateWhile(ASTNode* cond, ASTNode* body);
    ASTNode* CreateBreak();
    ASTNode* CreateCall(ASTNode* func, ASTNode* args);
    ASTNode* CreateReturn(ASTNode* ret_val);

private:
    ASTNode* root{};
    std::vector<SymbolTable> sym_tables;
    std::deque<ASTNode> node_pool;
    std::deque<std::string> string_pool;

    ASTNode* decl_seq_type{};
};