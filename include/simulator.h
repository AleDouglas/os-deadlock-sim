#ifndef SIMULATOR_H
#define SIMULATOR_H
/* ---------------------------------------------------------------------
 * simulator.h
 * Estrutura do sistema e interface de inicialização/reset/finalização.
 * --------------------------------------------------------------------- */

#include <stdint.h>
#include "resources.h"
#include "metrics.h"
#include "process.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================
 * Estrutura do sistema
 * ============================ */
typedef struct System {
    int      n;                                    /* # de processos ativos           */
    int      m;                                    /* # de tipos de recursos          */
    int      Available[MAX_R];                     /* instâncias livres por recurso   */
    Process  procs[MAX_P];                         /* tabela de processos             */
    Mode     mode;                                 /* BANKER ou OSTRICH               */
    uint64_t sim_clock;                            /* “tempo” lógico da simulação     */
    Metrics  metrics;                              /* contadores/tempo de execução    */
} System;

/* ============================
 * Interface do simulador
 * ============================ */
void sim_init(System *s, int n, int m, Mode mode);
void sim_reset(System *s);
void sim_finalize(System *s);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SIMULATOR_H */
