#ifndef PROCESS_H
#define PROCESS_H
/* ---------------------------------------------------------------------
 * process.h
 * Definição do processo e protótipos utilitários.
 * --------------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>
#include "resources.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Encaminhamento: lista de requisições (implementaremos depois) */
struct ReqList;

/* ============================
 * Estrutura do processo
 * ============================ */
typedef struct Process {
    int     id;                                   /* identificador do processo       */
    PState  state;                                 /* ciclo de vida                   */
    int     Max[MAX_R];                            /* demanda máxima por recurso      */
    int     Allocation[MAX_R];                     /* instâncias alocadas             */
    int     Need[MAX_R];                           /* Need = Max - Allocation         */
    struct  ReqList *script;                       /* sequência de requisições        */
    uint64_t wait_time_acc;                        /* tempo acumulado bloqueado       */
} Process;

/* ============================
 * Protótipos utilitários
 * ============================ */
void proc_reset(Process *p, int id);
void proc_compute_need(Process *p);
bool proc_invariants_ok(const Process *p);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROCESS_H */
