#ifndef __interpreter_h__
#define __interpreter_h__

#include "parser.h"

int eval_op(char *op, int x, int y);
int eval(AST *ast);

#endif // __interpreter_h__
