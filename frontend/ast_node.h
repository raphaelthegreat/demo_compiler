#pragma once

#include <memory>
#include <vector>
#include <string_view>

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