#include <print>
#include "parse_context.h"
#include "compiler.tab.h"

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

    ParseContext* context = new ParseContext;
    yyparse(context);
    context->PrintTree();
}