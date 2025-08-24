/* src/stubs.c — implementações temporárias só para linkar */
#include "resources.h"
#include "process.h"
#include "simulator.h"

/* Dispatcher temporário: ainda não implementado */
bool handle_request_current_mode(System *S, Process *P, const int req[MAX_R]) {
    (void)S; (void)P; (void)req;
    /* TODO: implementar Ostrich/Banker. Por enquanto, sempre nega. */
    return false;
}

/* Liberação simplificada (essa já é útil de verdade) */
void release_all_resources(System *S, Process *P) {
    if (!S || !P) return;
    for (int j = 0; j < S->m; ++j) {
        S->Available[j] += P->Allocation[j];
        P->Allocation[j] = 0;
        P->Need[j]       = 0;   /* ou: recompute depois com proc_compute_need(P) */
    }
}

/* Filas ainda não existem — stubs no-op */
void enqueue_ready(int pid)   { (void)pid; /* TODO: implementar fila READY */ }
void enqueue_blocked(int pid) { (void)pid; /* TODO: implementar fila BLOCKED */ }
int  blocked_count(void)      { return 0;  /* TODO: retornar tamanho real */ }
