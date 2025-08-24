#ifndef BANKER_H
#define BANKER_H
/* ---------------------------------------------------------------------
 * banker.h — Algoritmo do Banqueiro: request + safety check
 * --------------------------------------------------------------------- */
#include <stdbool.h>
#include "resources.h"
#include "simulator.h"  /* System */
#include "process.h"    /* Process */

#ifdef __cplusplus
extern "C" {
#endif

bool safety_check(const System *S);
bool request_banker(System *S, Process *P, const int req[MAX_R]);

#ifdef __cplusplus
}
#endif
#endif /* BANKER_H */
