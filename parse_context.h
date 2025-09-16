#pragma once

#include <array>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

struct SourceLoc {
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};

enum class BasicType {
    Void,
    Int,
};

enum class UnaryOp {
    Increment,
};

enum class BinaryOp {
    Assign,
    Add,
    Subtract,
    Multiply,
    Divide,
    LessThan,
	LessThanEqual,
	GreaterThan,
	GreaterThanEqual,
	Equal,
	NotEqual,
};

enum class NodeType {
    ConstInt,   // const_val
    Type,       // basic_type
    Function,   // type, params, name, body
    Symbol,     // type, name, initializer
    Block,      // children
    UnaryOp,    // unary_op, node_a
    BinaryOp,   // binary_op, node_a, node_b
    While,      // cond, body
    Break,      //
    IfElse,     // cond, if_body, else_body
    Call,       // func, args
    Return,     // ret_expr
};

struct ASTNode {
    NodeType node_type;
    std::unique_ptr<std::vector<ASTNode*>> children;
    union {
        BasicType basic_type;
        UnaryOp unary_op;
        BinaryOp binary_op;
        ASTNode* type;
        ASTNode* cond;
        ASTNode* func;
        ASTNode* ret_expr;
        int const_val;
    };
    union {
        ASTNode* params;
        ASTNode* node_a;
        ASTNode* if_body;
        ASTNode* args;
    };
    union {
        ASTNode* node_b;
        ASTNode* body;
        ASTNode* else_body;
        ASTNode* initializer;
    };
    std::string_view name;

    explicit ASTNode(NodeType type) : node_type(type) {}
    ~ASTNode() = default;
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