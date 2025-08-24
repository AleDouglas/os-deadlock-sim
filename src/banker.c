/* ---------------------------------------------------------------------
 * banker.c — Implementação do Algoritmo do Banqueiro
 * --------------------------------------------------------------------- */
#include <stdbool.h>
#include "banker.h"

static inline bool vec_leq_need(const int need[MAX_R], const int work[MAX_R], int m) {
    for (int j = 0; j < m; ++j) if (need[j] > work[j]) return false;
    return true;
}

bool safety_check(const System *S) {
    if (!S) return false;

    int  m = S->m, n = S->n;
    int  Work[MAX_R];
    bool Finish[MAX_P];

    /* Work = Available */
    for (int j = 0; j < m; ++j) Work[j] = S->Available[j];

    /* Finish[i] = (Need[i] == 0?) */
    for (int i = 0; i < n; ++i) {
        bool zero = true;
        for (int j = 0; j < m; ++j) if (S->procs[i].Need[j] != 0) { zero = false; break; }
        Finish[i] = zero;
    }

    bool progress = true;
    while (progress) {
        progress = false;
        for (int i = 0; i < n; ++i) {
            if (Finish[i]) continue;
            if (vec_leq_need(S->procs[i].Need, Work, m)) {
                /* Work += Allocation[i] */
                for (int j = 0; j < m; ++j) Work[j] += S->procs[i].Allocation[j];
                Finish[i] = true;
                progress = true;
            }
        }
    }

    /* Seguro se todos podem terminar */
    for (int i = 0; i < n; ++i) if (!Finish[i]) return false;
    return true;
}

bool request_banker(System *S, Process *P, const int req[MAX_R]) {
    if (!S || !P || !req) return false;

    /* 1) Checagens básicas */
    for (int j = 0; j < S->m; ++j) {
        int r = req[j];
        if (r < 0) return false;
        if (r > P->Need[j]) return false;
        if (r > S->Available[j]) return false;
    }

    /* 2) Tentativa (aplica provisoriamente) */
    for (int j = 0; j < S->m; ++j) {
        int r = req[j];
        S->Available[j]  -= r;
        P->Allocation[j] += r;
        P->Need[j]       -= r;
    }

    /* 3) Safety check */
    bool safe = safety_check(S);

    if (safe) {
        return true; /* mantém a tentativa */
    } else {
        /* 4) Rollback */
        for (int j = 0; j < S->m; ++j) {
            int r = req[j];
            S->Available[j]  += r;
            P->Allocation[j] -= r;
            P->Need[j]       += r;
        }
        return false;
    }
}
