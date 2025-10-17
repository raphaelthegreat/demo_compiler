#include <print>

#include "backend/mix/mix_emitter.h"
#include "frontend/parse_context.h"
#include "frontend/translate_pass.h"

#include "frontend/compiler_tab_autogen.h"

#include "ir/passes/passes.h"
#include "backend/mix/mix_ir_pass.h"

extern FILE *yyin;
extern int yylex (void);
extern int yyparse (ParseContext* context);

int main(int argc, char** argv) {
    if (argc != 2) {
        std::print("Usage: ./compiler program.txt\n");
        return 0;
    }

    FILE *myfile = fopen(argv[1], "r");
    if (!myfile) {
        std::print("Can't open input file {}\n", argv[1]);
        return -1;
    }
    yyin = myfile;

    // Parse text program and generate AST
    ParseContext context;
    yyparse(&context);
    context.PrintTree();
    auto* root = context.Root();

    // Emit IR from the generated AST
    TranslatePass translate_pass(root);
    auto& program = translate_pass.Program();

    // Run transformation passes on IR
    IR::LoadStoreElimination(program);
    Mix::MixPreprocessPass(program);
    IR::IdentityRemovalPass(program);

    // Emit MIX assembly
    Mix::Emitter emitter(program);

    // Dump IR
    for (auto& function : program.functions) {
        IR::DumpFunction(function);
    }
}