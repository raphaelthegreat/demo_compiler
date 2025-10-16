%{
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <deque>
#include <vector>
#include "parse_context.h"
%}

%parse-param { ParseContext* context }

%code requires {
#define YYLTYPE SourceLoc
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1
}

%union {
    char str[16];
    int val;
    ASTNode* node;
	BinaryOp binary_op;
}

%{
extern int yylex(void);
extern void yyerror(ParseContext* context, const char* reason);
%}

%token <val> INT IF ELSE WHILE RETURN BREAK
%token <val> NUM TRUE FALSE
%token <str> ID LE GE EQ NE GT LT
%token <str> '(' ')' '{' '}'
%token <str> '*' '/' '%' '+' '-' '=' ';' ','

%type <node> program function params args expr
%type <node> type body decl_list decl stmts stmt
%type <node> block add_expr term factor
%type <binary_op> relop addop mulop

%right '='
%left '<' '>' LE GE EQ NE LT GT
%start program
%%

program
	: program function {
		$$ = $1;
		$$->children->push_back($2);
	}
	| function {
		$$ = context->CreateBlock($1);
		context->SetRoot($$);
	}
	| {
		$$ = context->CreateBlock(nullptr);
		context->SetRoot($$);
	}
	;

function
	: type ID '(' params ')' body {
		context->Pop();
		$$ = context->CreateFunction(@1, $1, $2, $4, $6);
	} ;

params
	: type ID {
		context->Push();
		$$ = context->CreateBlock(context->CreateSymbol($1, $2));
	}
	| params ',' type ID {
		$$ = $1;
		$$->children->push_back(context->CreateSymbol($3, $4));
	}
	| {
		context->Push();
		$$ = context->CreateBlock(nullptr);
	}
	;

type: INT { $$ = context->CreateIntType(); } ;

body
	: '{' decl_list stmts '}' {
		$$ = context->CreateBlock($2);
		$$->children->push_back($3);
	}
	;

decl_list
	: decl {
		$$ = context->CreateBlock($1);
	}
	| decl_list decl {
		$$ = $1;
		$$->children->push_back($2);
	}
	| {
		$$ = context->CreateBlock(nullptr);
	}
	;

decl
	: type ID ';' {
		$$ = context->CreateSymbol($1, $2);
	}
	| type ID ',' {
		$$ = context->CreateSymbol($1, $2);
		context->SetDeclSeqType($1);
	}
	| type ID '=' expr ';' {
		$$ = context->CreateSymbol($1, $2, $4);
	}
	| type ID '=' expr ',' {
		$$ = context->CreateSymbol($1, $2, $4);
		context->SetDeclSeqType($1);
	}
	| ',' ID ';' {
		$$ = context->CreateSymbol(context->DeclSeqType(), $2);
		context->SetDeclSeqType(nullptr);
	}
	| ',' ID ',' {
		$$ = context->CreateSymbol(context->DeclSeqType(), $2);
	}
	| ',' ID '=' expr ';' {
		$$ = context->CreateSymbol(context->DeclSeqType(), $2, $4);
		context->SetDeclSeqType(nullptr);
	}
	| ',' ID '=' expr ',' {
		$$ = context->CreateSymbol(context->DeclSeqType(), $2, $4);
	}
	;

stmts
	: stmt {
		printf("BEGIN STMT\n");
		$$ = context->CreateBlock($1);
	}
	| stmts stmt {
		printf("ADD STMT\n");
		$$ = $1;
		if ($2) {
			$$->children->push_back($2);
		}
	}
	| {
		$$ = context->CreateBlock(nullptr);
	}
	;

stmt
	: ID '=' expr ';' {
		printf("MATCHED ASSIGN\n");
		$$ = context->CreateBinaryOp(BinaryOp::Assign, context->FindSymbol($1), $3);
	}
	| RETURN expr ';' {
		$$ = context->CreateReturn($2);
	}
	| IF '(' expr ')' block ELSE block {
		$$ = context->CreateIfElse($3, $5, $7);
	}
	| WHILE '(' expr ')' { printf("MATCHED WHILE HEADER\n"); } block {
		$$ = context->CreateWhile($3, $6);
	}
	| BREAK ';' {
		$$ = context->CreateBreak();
	}
	| block {
		printf("MATCHED BLOCK\n");
		$$ = $1;
	}
	| ';' {
		printf("MATCHED ;\n");
		$$ = nullptr;
	}
	;

block
	: '{' { context->Push(); printf("BEGIN BLOCK\n"); } stmts '}' {
		$$ = $3;
		context->Pop();
	}
	;

expr
	: add_expr relop add_expr {
		$$ = context->CreateBinaryOp($2, $1, $3);
	}
	| add_expr { 
		$$ = $1;
	}
	;

relop
	: LE { $$ = BinaryOp::LessThan; }
	| LT { $$ = BinaryOp::LessThanEqual; }
	| GT { $$ = BinaryOp::GreaterThan; }
	| GE { $$ = BinaryOp::GreaterThanEqual; }
	| EQ { $$ = BinaryOp::Equal; }
	| NE { $$ = BinaryOp::NotEqual; }
	;

add_expr
	: add_expr addop term {
		$$ = context->CreateBinaryOp($2, $1, $3);
	}
	| term {
		$$ = $1;
	}
	;

addop
	: '+' { $$ = BinaryOp::Add; }
	| '-' { $$ = BinaryOp::Subtract; }
	;

term
	: term mulop factor {
		$$ = context->CreateBinaryOp($2, $1, $3);
	}
	| factor {
		$$ = $1;
	}
	;

mulop
	: '*' { $$ = BinaryOp::Multiply; }
	| '/' { $$ = BinaryOp::Divide; }
	;

factor
	: '(' expr ')' {
		$$ = $2;
	}
	| ID {
		$$ = context->FindSymbol($1);
	}
	| NUM {
		$$ = context->CreateIntConst($1);
	}
	| TRUE {
		$$ = context->CreateIntConst(1);
	}
	| FALSE {
		$$ = context->CreateIntConst(0);
	}
	| ID '(' args ')' {
		$$ = context->CreateCall(context->FindSymbol($1), $3);
	}
	;

args
	: expr {
		$$ = context->CreateBlock($1);
	}
	| args ',' expr {
		$$ = $1;
		$$->children->push_back($3);
	}
	| {
		$$ = context->CreateBlock(nullptr);
	}
	;

%%

int compiler_parse(ParseContext* context) {
    return yyparse(context);
}