/* ---------------------------------------------------------------------
 * ostrich.c — Dispatcher do modo OSTRICH (ignora prevenção de deadlock)
 * Concede req se houver Available suficiente e sem ultrapassar Need.
 * --------------------------------------------------------------------- */
#include <stdbool.h>
#include "resources.h"
#include "simulator.h"
#include "process.h"

bool handle_request_current_mode(System *S, Process *P, const int req[MAX_R]) {
    if (!S || !P || !req) return false;

    /* Caso BANKER ainda não implementado: negar por enquanto */
    if (S->mode == MODE_BANKER) {
        S->metrics.total_requests++;
        S->metrics.blocks++;
        return false;
    }

    /* --------- MODO OSTRICH --------- */
    S->metrics.total_requests++;

    /* Checagens básicas em 0..m-1 */
    for (int j = 0; j < S->m; ++j) {
        int r = req[j];
        if (r < 0) {
            S->metrics.blocks++;
            return false;
        }
        if (r > P->Need[j]) {          /* não pode pedir além do Need */
            S->metrics.blocks++;
            return false;
        }
        if (r > S->Available[j]) {     /* não há instância livre suficiente */
            S->metrics.blocks++;
            return false;
        }
    }

    /* Concede: Available -= req; Allocation += req; Need -= req */
    for (int j = 0; j < S->m; ++j) {
        int r = req[j];
        S->Available[j]  -= r;
        P->Allocation[j] += r;
        P->Need[j]       -= r;
    }

    S->metrics.grants++;
    return true;
}
