#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;

// Token
typedef enum {
  TK_IDENT,   // Identifiers
  TK_PUNCT,   // Punctuators
  TK_KEYWORD, // Keywords
  TK_NUM,     // Numeric literals
  TK_EOF,     // End-of-file markers
} TokenKind;

typedef struct Token Token;

// // トークン型
// struct Token {
//   TokenKind kind;  // トークンの型
//   Token *next;     // 次の入力トークン
//   int val;         // kindがTK_NUMの場合、その数値
//   char *str;       // トークン文字列
//   int len;         // トークンの長さ
// };
struct Token {
  TokenKind kind; // Token kind
  Token *next;    // Next token
  int val;        // If kind is TK_NUM, its value
  char *loc;      // Token location
  int len;        // Token length
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
Token *tokenize(char *input);

//
// parse.c
//

// Local variable
typedef struct Obj Obj;
struct Obj {
  Obj *next;
  char *name; // Variable name
  int offset; // Offset from RBP
};

// Function
typedef struct Function Function;
struct Function {
  Node *body;
  Obj *locals;
  int stack_size;
};


// // 抽象構文木のノードの種類
// typedef enum {
//   ND_ADD,  // +
//   ND_SUB,  // -
//   ND_MUL,  // *
//   ND_DIV,  // /
//   ND_NUM,  // 整数
//   ND_EQ,   // ==
//   ND_NE,   // !=
//   ND_LT,   // <
//   ND_LE,   // <=
//   ND_ASSIGN, // =
//   ND_RETURN,    // "return"
//   ND_VAR,       // Variable
//   ND_NUM,       // Integer
// } NodeKind;

// AST node
typedef enum {
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_NEG,       // unary -
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_ASSIGN,    // =
  ND_RETURN,    // "return"
  ND_EXPR_STMT, // Expression statement
  ND_VAR,       // Variable
  ND_NUM,       // Integer
} NodeKind;



// // 抽象構文木のノードの型
// struct Node {
//   NodeKind kind;  // ノードの型
//   Node *lhs;      // 左辺
//   Node *rhs;      // 右辺
//   Node *next;    // Next node
//   Obj *var; // Used if kind == ND_VAR
//   char name;     // Used if kind == ND_LVAR
// };

// AST node type
struct Node {
  NodeKind kind; // Node kind
  Node *next;    // Next node
  Node *lhs;     // Left-hand side
  Node *rhs;     // Right-hand side
  Obj *var;      // Used if kind == ND_VAR
  int val;       // Used if kind == ND_NUM
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
Token *tokenize(char *input);

// // Node *code;
// Node *code[100];
// Node *program();
// Node *stmt();
// Node *assign();
// Node *expr();
// Node *equality();
// Node *relational();
// Node *add();
// Node *mul();
// Node *unary();
// Node *primary();

// // 現在のトークン
// Token *token;
// // インプット
// char *user_input;

// void error(char *fmt, ...);
// bool consume(char *op);
// Token *consume_ident();
// void expect(char *op);
// int expect_number();
// bool at_eof();
// Token *new_token(TokenKind kind, Token *current, char *str, int len);
// Token *tokenize();

Function *parse(Token *tok);

// void gen(Node *node);
void codegen(Function *prog);