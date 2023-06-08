#include "evaluator.h"
#include "types.h"
#include "butils.h"


enum opE {
  OP_ADD,
  OP_SUB,
  OP_DIV,
  OP_MUL,
  OP_POW,
  OP_MOD,
  OP_LPARENT,
  OP_RPARENT,
} typedef opE;

enum itemE {
  OPERATOR,
  INT_NUM,
  FLOAT_NUM,
  VARIABLE,
  FUNCTION,
} typedef itemE;

union itemU {
  opE op_id;
  s64 int_val;
  // float float_val;
  // char* varname;
  // char* funname;
} typedef itemU;

struct itemS {
  itemE item_type;
  itemU item_val;
} typedef itemS;


s64 eval_int(char* expr) {
  // opE opstack[32];
  // int oi = 0;
  // itemS rpn[64];
  // int rpnidx = 0;

  // for (int i=0; expr[i] != '\0'; ++i) {
  //   if (isdigit(expr[i])) {
  //     rpn[rpnidx].item_type = INT_NUM;
  //     rpn[rpnidx++].item_val.int_val = str2s64(expr[i]);
  //   }
  //   if (expr[i] == '+') {
  //     if (opstack[oi] == '+' || opstack[oi] == '-') {

  //     } else {
  //       opstack[oi++]
  //     }
  //   }
  // }

  return 0;
}