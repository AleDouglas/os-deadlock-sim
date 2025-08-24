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

/* Encaminhamento: lista de requisições */
typedef struct ReqList{
    int len;
    int idx;
    int items[MAX_REQS][MAX_R];  /* cada item é um vetor de requisição */
} ReqList;


/* ============================
 * Protótipos utilitários
 * ============================ */
void reqlist_init(ReqList *R);
static inline void reqlist_reset(ReqList *R) { reqlist_init(R); }
bool reqlist_empty(const ReqList *R);
int  reqlist_count(const ReqList *R);               /* remanescentes (len - idx) */
bool reqlist_push(ReqList *R, const int *req, int m);
bool reqlist_peek(const ReqList *R, int out_req[MAX_R]);
bool reqlist_pop(ReqList *R);
void reqlist_rewind(ReqList *R);

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
