#include "calc.h"

// All local variable instances created during parsing are
// accumulated to this list.
Obj *locals;

static Node *expr(Token **rest, Token *tok);
static Node *expr_stmt(Token **rest, Token *tok);
static Node *assign(Token **rest, Token *tok);
static Node *equality(Token **rest, Token *tok);
static Node *relational(Token **rest, Token *tok);
static Node *add(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *unary(Token **rest, Token *tok);
static Node *primary(Token **rest, Token *tok);

// Node *new_node(NodeKind kind) {
//   Node *node = calloc(1, sizeof(Node));
//   node->kind = kind;
//   return node;
// }

// Find a local variable by name.
static Obj *find_var(Token *tok) {
  for (Obj *var = locals; var; var = var->next)
    if (strlen(var->name) == tok->len &&
        !strncmp(tok->loc, var->name, tok->len))
      return var;
  return NULL;
}

static Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_unary(NodeKind kind, Node *expr) {
  Node *node = new_node(kind);
  node->lhs = expr;
  return node;
}

static Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

// Node *new_lvar(char name) {
//   Node *node = new_node(ND_LVAR);
//   node->name = name;
//   return node;
// }

static Node *new_var_node(Obj *var) {
  Node *node = new_node(ND_VAR);
  node->var = var;
  return node;
}

static Obj *new_lvar(char *name) {
  Obj *var = calloc(1, sizeof(Obj));
  var->name = name;
  var->next = locals;
  locals = var;
  return var;
}

// Node *stmt();
// Node *expr();
// Node *assign();
// Node *equality();
// Node *relational();
// Node *add();
// Node *mul();
// Node *unary();
// Node *primary();

// program = stmt*
Node *program() {
  Node head;
  head.next = NULL;
  Node *cur = &head;

  while (!at_eof()) {
    cur->next = stmt();
    cur = cur->next;
  }
  return head.next;
}

// // stmt = "return" expr ";"
// //      | expr ";"
// Node *stmt() {
//   if (consume("return")) {
//     Node *node = new_unary(ND_RETURN, expr());
//     expect(";");
//     return node;
//   }

//   Node *node = new_unary(ND_EXPR_STMT, expr());
//   expect(";");
//   return node;
// }

// stmt = "return" expr ";"
//      | expr-stmt
static Node *stmt(Token **rest, Token *tok) {
  if (equal(tok, "return")) {
    Node *node = new_unary(ND_RETURN, expr(&tok, tok->next));
    *rest = skip(tok, ";");
    return node;
  }

  return expr_stmt(rest, tok);
}

// expr-stmt = expr ";"
static Node *expr_stmt(Token **rest, Token *tok) {
  Node *node = new_unary(ND_EXPR_STMT, expr(&tok, tok));
  *rest = skip(tok, ";");
  return node;
}

// // expr = assign
// Node *expr() { return assign(); }

// expr = assign
static Node *expr(Token **rest, Token *tok) { return assign(rest, tok); }

// // assign = equality ("=" assign)?
// Node *assign() {
//   Node *node = equality();
//   if (consume("=")) node = new_binary(ND_ASSIGN, node, assign());
//   return node;
// }

// assign = equality ("=" assign)?
static Node *assign(Token **rest, Token *tok) {
  Node *node = equality(&tok, tok);
  if (equal(tok, "="))
    node = new_binary(ND_ASSIGN, node, assign(&tok, tok->next));
  *rest = tok;
  return node;
}

// // equality = relational ("==" relational | "!=" relational)*
// Node *equality() {
//   Node *node = relational();

//   for (;;) {
//     if (consume("=="))
//       node = new_binary(ND_EQ, node, relational());
//     else if (consume("!="))
//       node = new_binary(ND_NE, node, relational());
//     else
//       return node;
//   }
// }

// equality = relational ("==" relational | "!=" relational)*
static Node *equality(Token **rest, Token *tok) {
  Node *node = relational(&tok, tok);

  for (;;) {
    if (equal(tok, "==")) {
      node = new_binary(ND_EQ, node, relational(&tok, tok->next));
      continue;
    }

    if (equal(tok, "!=")) {
      node = new_binary(ND_NE, node, relational(&tok, tok->next));
      continue;
    }

    *rest = tok;
    return node;
  }
}

// // relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// Node *relational() {
//   Node *node = add();

//   for (;;) {
//     if (consume("<"))
//       node = new_binary(ND_LT, node, add());
//     else if (consume("<="))
//       node = new_binary(ND_LE, node, add());
//     else if (consume(">"))
//       node = new_binary(ND_LT, add(), node);
//     else if (consume(">="))
//       node = new_binary(ND_LE, add(), node);
//     else
//       return node;
//   }
// }

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static Node *relational(Token **rest, Token *tok) {
  Node *node = add(&tok, tok);

  for (;;) {
    if (equal(tok, "<")) {
      node = new_binary(ND_LT, node, add(&tok, tok->next));
      continue;
    }

    if (equal(tok, "<=")) {
      node = new_binary(ND_LE, node, add(&tok, tok->next));
      continue;
    }

    if (equal(tok, ">")) {
      node = new_binary(ND_LT, add(&tok, tok->next), node);
      continue;
    }

    if (equal(tok, ">=")) {
      node = new_binary(ND_LE, add(&tok, tok->next), node);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// // add = mul ("+" mul | "-" mul)*
// Node *add() {
//   Node *node = mul();

//   for (;;) {
//     if (consume("+"))
//       node = new_binary(ND_ADD, node, mul());
//     else if (consume("-"))
//       node = new_binary(ND_SUB, node, mul());
//     else
//       return node;
//   }
// }

// add = mul ("+" mul | "-" mul)*
static Node *add(Token **rest, Token *tok) {
  Node *node = mul(&tok, tok);

  for (;;) {
    if (equal(tok, "+")) {
      node = new_binary(ND_ADD, node, mul(&tok, tok->next));
      continue;
    }

    if (equal(tok, "-")) {
      node = new_binary(ND_SUB, node, mul(&tok, tok->next));
      continue;
    }

    *rest = tok;
    return node;
  }
}

// // mul = unary ("*" unary | "/" unary)*
// Node *mul() {
//   Node *node = unary();

//   for (;;) {
//     if (consume("*"))
//       node = new_binary(ND_MUL, node, unary());
//     else if (consume("/"))
//       node = new_binary(ND_DIV, node, unary());
//     else
//       return node;
//   }
// }

// mul = unary ("*" unary | "/" unary)*
static Node *mul(Token **rest, Token *tok) {
  Node *node = unary(&tok, tok);

  for (;;) {
    if (equal(tok, "*")) {
      node = new_binary(ND_MUL, node, unary(&tok, tok->next));
      continue;
    }

    if (equal(tok, "/")) {
      node = new_binary(ND_DIV, node, unary(&tok, tok->next));
      continue;
    }

    *rest = tok;
    return node;
  }
}

// // unary = ("+" | "-")? unary
// //       | primary
// Node *unary() {
//   if (consume("+")) return unary();
//   if (consume("-")) return new_binary(ND_SUB, new_num(0), unary());
//   return primary();
// }

// unary = ("+" | "-") unary
//       | primary
static Node *unary(Token **rest, Token *tok) {
  if (equal(tok, "+")) return unary(rest, tok->next);

  if (equal(tok, "-")) return new_unary(ND_NEG, unary(rest, tok->next));

  return primary(rest, tok);
}

// // primary = "(" expr ")" | ident | num
// Node *primary() {
//   if (consume("(")) {
//     Node *node = expr();
//     expect(")");
//     return node;
//   }

//   Token *tok = consume_ident();
//   if (tok) return new_lvar(*tok->str);

//   return new_num(expect_number());
// }

// primary = "(" expr ")" | ident | num
static Node *primary(Token **rest, Token *tok) {
  if (equal(tok, "(")) {
    Node *node = expr(&tok, tok->next);
    *rest = skip(tok, ")");
    return node;
  }

  if (tok->kind == TK_IDENT) {
    Obj *var = find_var(tok);
    if (!var) var = new_lvar(strndup(tok->loc, tok->len));
    *rest = tok->next;
    return new_var_node(var);
  }

  if (tok->kind == TK_NUM) {
    Node *node = new_num(tok->val);
    *rest = tok->next;
    return node;
  }

  error_tok(tok, "expected an expression");
}

// program = stmt*
Function *parse(Token *tok) {
  Node head = {};
  Node *cur = &head;

  while (tok->kind != TK_EOF) cur = cur->next = stmt(&tok, tok);

  Function *prog = calloc(1, sizeof(Function));
  prog->body = head.next;
  prog->locals = locals;
  return prog;
}