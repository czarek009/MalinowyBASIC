#ifndef _EVALUATOR_H
#define _EVALUATOR_H

#include "types.h"
#include "session.h"
#include "variable.h"

u8 eval_expr(sessionS *s, char** expr, variableDataU *result);
u8 *eval_array_sizes(sessionS *s, char** cmd, u8 dimentions);

#endif /* _EVALUATOR_H */