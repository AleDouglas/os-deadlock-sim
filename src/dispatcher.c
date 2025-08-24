/* ---------------------------------------------------------------------
 * dispatcher.c — Decide concessão conforme o modo (OSTRICH ou BANKER)
 * Mede overhead do BANKER (ns) e atualiza métricas.
 * --------------------------------------------------------------------- */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <time.h>
#include "resources.h"
#include "simulator.h"
#include "process.h"
#include "banker.h"
#include "logger.h"

static inline unsigned long long now_ns(void) {
    struct timespec ts;
#if defined(CLOCK_MONOTONIC_RAW)
    /* Usa RAW quando disponível (Linux), mais estável contra NTP */
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
#else
    /* Fallback POSIX portátil */
    clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
    return (unsigned long long)ts.tv_sec * 1000000000ull
         + (unsigned long long)ts.tv_nsec;
}

bool handle_request_current_mode(System *S, Process *P, const int req[MAX_R]) {
    if (!S || !P || !req) return false;
    S->metrics.total_requests++;

    /* ---------- BANKER ---------- */
    if (S->mode == MODE_BANKER) {
        /* pré-checagem: só mede safety se possível prosseguir */
        for (int j = 0; j < S->m; ++j) {
            int r = req[j];
            if (r < 0 || r > P->Need[j] || r > S->Available[j]) {
                S->metrics.blocks++;
                logger_log_request(S, P, req, false);
                return false;
            }
        }
        unsigned long long t0 = now_ns();
        bool ok = request_banker(S, P, req);
        unsigned long long dt = now_ns() - t0;

        S->metrics.banker_safety_calls++;
        S->metrics.ns_in_safety_total += dt;
        if (ok) S->metrics.grants++; else S->metrics.blocks++;
        logger_log_request(S, P, req, ok);
        return ok;
    }

    /* ---------- OSTRICH ---------- */
    for (int j = 0; j < S->m; ++j) {
        int r = req[j];
        if (r < 0 || r > P->Need[j] || r > S->Available[j]) {
            S->metrics.blocks++;
            logger_log_request(S, P, req, false);
            return false;
        }
    }
    for (int j = 0; j < S->m; ++j) {
        int r = req[j];
        S->Available[j]  -= r;
        P->Allocation[j] += r;
        P->Need[j]       -= r;
    }
    S->metrics.grants++;
    logger_log_request(S, P, req, true);
    return true;
}