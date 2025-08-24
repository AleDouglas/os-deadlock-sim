/* ---------------------------------------------------------------------
 * detector.c — Detecção de deadlock (variação do safety)
 * Não modifica o estado do sistema.
 * --------------------------------------------------------------------- */
#include <stdbool.h>
#include "detector.h"

static inline bool need_leq_work(const int need[MAX_R], const int work[MAX_R], int m) {
    for (int j = 0; j < m; ++j) if (need[j] > work[j]) return false;
    return true;
}

bool detect_deadlock(const System *S) {
    if (!S) return false;

    int  m = S->m, n = S->n;
    int  Work[MAX_R];
    bool Finish[MAX_P];

    for (int j = 0; j < m; ++j) Work[j] = S->Available[j];

    for (int i = 0; i < n; ++i) {
        bool zero = true;
        for (int j = 0; j < m; ++j) if (S->procs[i].Need[j] != 0) { zero = false; break; }
        Finish[i] = zero; /* quem não precisa de nada já é “finalizável” */
    }

    bool progress = true;
    while (progress) {
        progress = false;
        for (int i = 0; i < n; ++i) {
            if (Finish[i]) continue;
            if (need_leq_work(S->procs[i].Need, Work, m)) {
                for (int j = 0; j < m; ++j) Work[j] += S->procs[i].Allocation[j];
                Finish[i] = true;
                progress = true;
            }
        }
    }

    /* deadlock se existir alguém não-finalizável */
    for (int i = 0; i < n; ++i) if (!Finish[i]) return true;
    return false;
}
