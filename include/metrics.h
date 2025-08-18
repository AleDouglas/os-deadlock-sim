#ifndef METRICS_H
#define METRICS_H
/* ---------------------------------------------------------------------
 * metrics.h
 * Contadores e utilitários de métricas do simulador.
 * --------------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Metrics {
    uint64_t total_requests;        /* nº total de requisições (qualquer modo)           */
    uint64_t banker_safety_calls;   /* nº de chamadas ao SafetyCheck (modo BANKER)       */
    uint64_t ns_in_safety_total;    /* tempo acumulado (ns) gasto em SafetyCheck         */
    uint64_t grants;                /* requisições concedidas                            */
    uint64_t blocks;                /* requisições bloqueadas/negadas                    */

    /* Modo OSTRICH (para relatório) */
    uint64_t deadlocks_found;       /* quantos deadlocks o detector encontrou            */
    uint64_t time_to_first_deadlock;/* “tempo lógico” até o 1º deadlock (0 = não houve)  */
} Metrics;

/* ============================
 * Helpers inline (sem .c)
 * ============================ */
static inline void metrics_reset(Metrics *m) {
    m->total_requests = 0;
    m->banker_safety_calls = 0;
    m->ns_in_safety_total = 0;
    m->grants = 0;
    m->blocks = 0;
    m->deadlocks_found = 0;
    m->time_to_first_deadlock = 0;
}

static inline void metrics_record_request(Metrics *m) {
    m->total_requests++;
}

static inline void metrics_record_safety_call(Metrics *m, uint64_t elapsed_ns) {
    m->banker_safety_calls++;
    m->ns_in_safety_total += elapsed_ns;
}

static inline void metrics_record_grant(Metrics *m) {
    m->grants++;
}

static inline void metrics_record_block(Metrics *m) {
    m->blocks++;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* METRICS_H */
