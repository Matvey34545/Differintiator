#ifndef DIFFERENTIATION
#define DIFFERENTIATION

#include "bynar_tree.h"
#include "read_expression.h"

#define dL(top, var) differentiation((top)->left, var)
#define dR(top, var) differentiation((top)->right, var)
#define cL(top) copy_tree((top)->left)
#define cR(top) copy_tree((top)->right)

#define _NUM(number)              CreateTop(CONST, MUL, number, NULL, NULL)
#define _PLUS(left, right)        CreateTop(OP, PLUS,  NULL, left, right)
#define _MINUS(left, right)       CreateTop(OP, MINUS,  NULL, left, right)
#define _MUL(left, right)         CreateTop(OP, MUL,  NULL, left, right)
#define _DIV(left, right)         CreateTop(OP, DIV,  NULL, left, right)
#define _DEGREE(left, right)      CreateTop(OP, DEGREE,  NULL, left, right)
#define _COS(left)                CreateTop(OP, COS,  NULL, left, NULL)
#define _SIN(left)                CreateTop(OP, SIN,  NULL, left, NULL)
#define _SQRT(left)               CreateTop(OP, SQRT,  NULL, left, NULL)
#define _SH(left)                 CreateTop(OP, SH,  NULL, left, NULL)
#define _CH(left)                 CreateTop(OP, CH,  NULL, left, NULL)
#define _LN(left)                 CreateTop(OP, LN,  NULL, left, NULL)
#define _EXP(left)                CreateTop(OP, EXP,  NULL, left, NULL)


void diff_and_simplifies_expressions(BynarTree *src_tree, BynarTree *diff_tree);

#endif
