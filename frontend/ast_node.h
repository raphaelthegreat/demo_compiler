#pragma once

#include <bit>
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
    DeclBlock,  // children
    Block,      // children
    UnaryOp,    // unary_op, node_a
    BinaryOp,   // binary_op, node_a, node_b
    While,      // cond, body
    Break,      //
    IfElse,     // cond, if_body, else_body
    Call,       // func, args
    Return,     // ret_expr
};

constexpr uint64_t NoPayload = -1LL;

struct ASTNode {
    NodeType node_type;
    ASTNode* parent{};
    std::unique_ptr<std::vector<ASTNode*>> children;
    uint64_t payload{NoPayload};
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

    void AddChild(ASTNode* child) {
        child->parent = this;
        children->push_back(child);
    }

    template <typename T = uint64_t>
    T Payload() const {
        return std::bit_cast<T>(payload);
    }

    template <typename T = uint64_t>
    void SetPayload(T val) {
        payload = std::bit_cast<uint64_t>(val);
    }
};