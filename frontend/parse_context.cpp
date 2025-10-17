#include <print>
#include <stack>

#include "parse_context.h"

void yyerror(ParseContext* context, const char* reason) {
    printf("%s\n", reason);
}

std::string DumpExpr(const ASTNode* expr) {
    if (expr->node_type == NodeType::ConstInt) {
        return std::format("{}", expr->const_val);
    }
    if (expr->node_type == NodeType::Symbol) {
        return std::format("{}", expr->name);
    }
    if (expr->node_type == NodeType::Call) {
        std::string params;
        auto* func = expr->func;
        auto& func_params = *expr->params->children;
        for (int i = 0; i < func_params.size(); ++i) {
            const auto* param = func_params[i];
            const bool is_last = i == func_params.size() - 1;
            params += DumpExpr(param);
            if (!is_last) {
                params += ", ";
            }
        }
        return std::format("{}({})", func->name, params);
    }
    if (expr->node_type != NodeType::BinaryOp) {
        std::abort();
    }
    std::string ret;
    switch (expr->binary_op) {
    case BinaryOp::Assign:
        ret += std::format("{} = {}", expr->node_a->name, DumpExpr(expr->node_b));
        break;
    case BinaryOp::GreaterThan:
        ret += std::format("({} > {})", DumpExpr(expr->node_a), DumpExpr(expr->node_b));
        break;
    case BinaryOp::Add:
        ret += std::format("({} + {})", DumpExpr(expr->node_a), DumpExpr(expr->node_b));
        break;
    case BinaryOp::Subtract:
        ret += std::format("({} - {})", DumpExpr(expr->node_a), DumpExpr(expr->node_b));
        break;
    case BinaryOp::Multiply:
        ret += std::format("({} * {})", DumpExpr(expr->node_a), DumpExpr(expr->node_b));
        break;
    case BinaryOp::Divide:
        ret += std::format("({} / {})", DumpExpr(expr->node_a), DumpExpr(expr->node_b));
        break;
    default:
        std::abort();
    }
    return ret;
}

std::string DumpBlock(int indentation, ASTNode* node) {
    if (node->node_type != NodeType::Block && node->node_type != NodeType::DeclBlock) {
        std::abort();
    }
    std::string indent(indentation, ' ');
    std::string ret;
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
                params += std::format("int {}", param->name);
                if (!is_last) {
                    params += ", ";
                }
            }
            ret += std::format("{}int {}({}) {{\n", indent, child->name, params);
            ret += DumpBlock(indentation + 2, child->body);
            ret += std::format("{}}}\n", indent);
            break;
        }
        case NodeType::Block:      // children
        case NodeType::DeclBlock:
            ret += DumpBlock(indentation + 2, child);
            break;
        case NodeType::UnaryOp:
            std::abort();
            break;
        case NodeType::BinaryOp:   // binary_op, node_a, node_b
            ret += std::format("{}{};\n", indent, DumpExpr(child));
            break;
        case NodeType::While:      // cond, body
            ret += std::format("{}while({}) {{\n", indent, DumpExpr(child->cond));
            ret += DumpBlock(indentation + 2, child->body);
            ret += std::format("{}}}\n", indent);
            break;
        case NodeType::Break:      //
            ret += indent + "break;\n";
            break;
        case NodeType::IfElse:     // cond, if_body, else_body
            ret += std::format("{}if({}) {{\n", indent, DumpExpr(child->cond));
            ret += DumpBlock(indentation + 2, child->if_body);
            ret += std::format("{}}} else {{\n", indent);
            ret += DumpBlock(indentation + 2, child->else_body);
            ret += std::format("{}}}\n", indent);
            break;
        case NodeType::Return: {    // ret_expr
            ret += std::format("{}return", indent);
            if (child->ret_expr) {
                auto* ret_expr = child->ret_expr;
                ret += std::format(" {};\n", DumpExpr(ret_expr));
            } else {
                ret += ";\n";
            }
            break;
        }
        case NodeType::Symbol:
            ret += std::format("{}int {}", indent, child->name);
            if (child->initializer) {
                ret += std::format(" = {};\n", DumpExpr(child->initializer));
            } else {
                ret += ";\n";
            }
            break;
        default:
            std::abort();
        }
    }

    return ret;
}

void ParseContext::PrintTree() {
    int indentation = 0;
    const auto ret = DumpBlock(indentation, root);
    std::print("\n\n{}", ret);
}

ASTNode* ParseContext::FindSymbol(const std::string& name) {
    int64_t i = sym_tables.size() - 1;
    while (i >= 0) {
        auto& sym_table = sym_tables[i];
        const auto it = sym_table.find(name);
        if (it != sym_table.end()) {
            return it->second;
        }
        --i;
    }
    std::print("Undefined name '{}'\n", name);
    std::abort();
}

ASTNode* ParseContext::CreateIntType() {
    auto* node = &node_pool.emplace_back(NodeType::Type);
    node->basic_type = BasicType::Int;
    return node;
}

ASTNode* ParseContext::CreateIntConst(int val) {
    auto* node = &node_pool.emplace_back(NodeType::ConstInt);
    node->const_val = val;
    return node;
}

ASTNode* ParseContext::CreateSymbol(ASTNode* type, const std::string& name,
                                    ASTNode* initializer) {
    auto& pooled_name = string_pool.emplace_back(std::move(name));
    auto* node = &node_pool.emplace_back(NodeType::Symbol);
    node->type = type;
    node->name = pooled_name;
    if (initializer) {
        node->initializer = initializer;
    }
    auto& table = sym_tables.back();
    table.emplace(name, node);
    return node;
}

ASTNode* ParseContext::CreateFunction(const SourceLoc& loc, ASTNode* return_type,
                                      const std::string& name, ASTNode* params,
                                      ASTNode* body) {
    const auto it = sym_tables[0].find(name);
    if (it != sym_tables[0].end()) {
        std::print("{}: Function name {} is already defined\n", 1, name);
        std::abort();
    }

    auto& pooled_name = string_pool.emplace_back(std::move(name));
    auto* node = &node_pool.emplace_back(NodeType::Function);
    node->type = return_type;
    node->name = pooled_name;
    node->params = params;
    node->body = body;
    auto& table = sym_tables.back();
    table.emplace(name, node);
    return node;
}

ASTNode* ParseContext::CreateBlock(ASTNode* initial_child) {
    auto* node = &node_pool.emplace_back(NodeType::Block);
    node->children = std::make_unique<std::vector<ASTNode*>>();
    if (initial_child) {
        node->AddChild(initial_child);
    }
    return node;
}

ASTNode* ParseContext::CreateDeclBlock(ASTNode* initial_child) {
    auto* node = &node_pool.emplace_back(NodeType::DeclBlock);
    node->children = std::make_unique<std::vector<ASTNode*>>();
    if (initial_child) {
        node->AddChild(initial_child);
    }
    return node;
}

ASTNode* ParseContext::CreateBinaryOp(BinaryOp op, ASTNode* node_a, ASTNode* node_b) {
    auto* node = &node_pool.emplace_back(NodeType::BinaryOp);
    node->binary_op = op;
    node->node_a = node_a;
    node->node_b = node_b;
    return node;
}

ASTNode* ParseContext::CreateIfElse(ASTNode* cond, ASTNode* if_body, ASTNode* else_body) {
    auto* node = &node_pool.emplace_back(NodeType::IfElse);
    node->cond = cond;
    node->if_body = if_body;
    node->if_body->parent = node;
    node->else_body = else_body;
    node->else_body->parent = node;
    return node;
}

ASTNode* ParseContext::CreateWhile(ASTNode* cond, ASTNode* body) {
    auto* node = &node_pool.emplace_back(NodeType::While);
    node->cond = cond;
    node->body = body;
    node->body->parent = node;
    return node;
}

ASTNode* ParseContext::CreateBreak() {
    return &node_pool.emplace_back(NodeType::Break);
}

ASTNode* ParseContext::CreateCall(ASTNode* func, ASTNode* args) {
    auto* node = &node_pool.emplace_back(NodeType::Call);
    node->func = func;
    node->args = args;
    return node;
}

ASTNode* ParseContext::CreateReturn(ASTNode* ret_val) {
    auto* node = &node_pool.emplace_back(NodeType::Return);
    node->ret_expr = ret_val;
    return node;
}