#include "calc.h"

static int depth;

static void push(void) {
  printf("  push %%rax\n");
  depth++;
}

static void pop(char *arg) {
  printf("  pop %s\n", arg);
  depth--;
}

// Compute the absolute address of a given node.
// It's an error if a given node does not reside in memory.
static int align_to(int n, int align) {
  return (n + align - 1) / align * align;
}

// Pushes the given node's address to the stack.
void gen_addr(Node *node) {
  if (node->kind == ND_VAR) {
    printf("  lea %d(%%rbp), %%rax\n", node->var->offset);
    return;
  }
  // if (node->kind == ND_LVAR) {
  //   int offset = (node->name - 'a' + 1) * 8;
  //   printf("  lea rax, [rbp-%d]\n", offset);
  //   printf("  push rax\n");
  //   return;
  // }

  error("not an lvalue");
}

// void load() {
//   printf("  pop rax\n");
//   printf("  mov rax, [rax]\n");
//   printf("  push rax\n");
// }

// void store() {
//   printf("  pop rdi\n");
//   printf("  pop rax\n");
//   printf("  mov [rax], rdi\n");
//   printf("  push rdi\n");
// }

// Generate code for a given node.
static void gen_expr(Node *node) {
  switch (node->kind) {
    case ND_NUM:
      printf("  mov $%d, %%rax\n", node->val);
      return;
    case ND_NEG:
      gen_expr(node->lhs);
      printf("  neg %%rax\n");
      return;
    case ND_VAR:
      gen_addr(node);
      printf("  mov (%%rax), %%rax\n");
      return;
    case ND_ASSIGN:
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      pop("%rdi");
      printf("  mov %%rax, (%%rdi)\n");
      return;
  }

  gen_expr(node->rhs);
  push();
  gen_expr(node->lhs);
  pop("%rdi");

  switch (node->kind) {
    case ND_ADD:
      printf("  add %%rdi, %%rax\n");
      return;
    case ND_SUB:
      printf("  sub %%rdi, %%rax\n");
      return;
    case ND_MUL:
      printf("  imul %%rdi, %%rax\n");
      return;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv %%rdi\n");
      return;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
      printf("  cmp %%rdi, %%rax\n");

      if (node->kind == ND_EQ)
        printf("  sete %%al\n");
      else if (node->kind == ND_NE)
        printf("  setne %%al\n");
      else if (node->kind == ND_LT)
        printf("  setl %%al\n");
      else if (node->kind == ND_LE)
        printf("  setle %%al\n");

      printf("  movzb %%al, %%rax\n");
      return;
  }

  error("invalid expression");
}

static void gen_stmt(Node *node) {
  // if (node->kind == ND_EXPR_STMT) {
  //   gen_expr(node->lhs);
  //   return;
  // }
  switch (node->kind) {
    case ND_RETURN:
      gen_expr(node->lhs);
      printf("  jmp .L.return\n");
      return;
    case ND_EXPR_STMT:
      gen_expr(node->lhs);
      return;
  }

  error("invalid statement");
}

// Assign offsets to local variables.
static void assign_lvar_offsets(Function *prog) {
  int offset = 0;
  for (Obj *var = prog->locals; var; var = var->next) {
    offset += 8;
    var->offset = -offset;
  }
  prog->stack_size = align_to(offset, 16);
}

void codegen(Function *prog) {
  assign_lvar_offsets(prog);

  printf("  .globl main\n");
  printf("main:\n");

  // Prologue
  printf("  push %%rbp\n");
  printf("  mov %%rsp, %%rbp\n");
  printf("  sub $%d, %%rsp\n", prog->stack_size);

  for (Node *n = prog->body; n; n = n->next) {
    gen_stmt(n);
    assert(depth == 0);
  }

  printf("  mov %%rbp, %%rsp\n");
  printf("  pop %%rbp\n");
  printf("  ret\n");
}

// // Generate code for a given node.
// void gen(Node *node) {
//   switch (node->kind) {
//     case ND_NUM:
//       printf("  push %d\n", node->val);
//       return;
//     case ND_EXPR_STMT:
//       gen(node->lhs);
//       printf("  add rsp, 8\n");
//       return;
//     case ND_LVAR:
//       gen_addr(node);
//       load();
//       return;
//     case ND_ASSIGN:
//       gen_addr(node->lhs);
//       gen(node->rhs);
//       store();
//       return;
//     case ND_RETURN:
//       gen(node->lhs);
//       printf("  pop rax\n");
//       printf("  jmp .Lreturn\n");
//       return;
//   }

//   gen(node->lhs);
//   gen(node->rhs);

//   printf("  pop rdi\n");
//   printf("  pop rax\n");

//   switch (node->kind) {
//     case ND_ADD:
//       printf("  add rax, rdi\n");
//       break;
//     case ND_SUB:
//       printf("  sub rax, rdi\n");
//       break;
//     case ND_MUL:
//       printf("  imul rax, rdi\n");
//       break;
//     case ND_DIV:
//       printf("  cqo\n");
//       printf("  idiv rdi\n");
//       break;
//     case ND_EQ:
//       printf("  cmp rax, rdi\n");
//       printf("  sete al\n");
//       printf("  movzb rax, al\n");
//       break;
//     case ND_NE:
//       printf("  cmp rax, rdi\n");
//       printf("  setne al\n");
//       printf("  movzb rax, al\n");
//       break;
//     case ND_LT:
//       printf("  cmp rax, rdi\n");
//       printf("  setl al\n");
//       printf("  movzb rax, al\n");
//       break;
//     case ND_LE:
//       printf("  cmp rax, rdi\n");
//       printf("  setle al\n");
//       printf("  movzb rax, al\n");
//       break;
//   }

//   printf("  push rax\n");
// }