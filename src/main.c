/* ---------------------------------------------------------------------
 * src/main.c
 * Teste do sys_load_from_arrays com um cenário tiny em memória.
 * --------------------------------------------------------------------- */
#include <stdio.h>
#include "simulator.h"
#include "process.h"   /* para proc_invariants_ok */

static const char* mode_str(Mode m) {
    return (m == MODE_BANKER) ? "BANKER" : "OSTRICH";
}
static const char* pstate_str(PState s) {
    switch (s) {
        case P_NEW: return "NEW";
        case P_READY: return "READY";
        case P_RUNNING: return "RUNNING";
        case P_BLOCKED: return "BLOCKED";
        case P_FINISHED: return "FINISHED";
        default: return "?";
    }
}
static void print_vec(const char *label, const int *v, int m) {
    printf("%s=[", label);
    for (int j = 0; j < m; ++j) {
        printf("%d%s", v[j], (j+1<m)?",":"");
    }
    printf("]\n");
}

int main(void) {
    /* 1) Inicializa o sistema vazio: 2 processos, 2 tipos de recurso, modo OSTRICH */
    System s;
    sim_init(&s, 2, 2, MODE_OSTRICH);
    printf("[init] n=%d m=%d mode=%s clock=%llu\n",
           s.n, s.m, mode_str(s.mode), (unsigned long long)s.sim_clock);

    /* 2) Define um cenário tiny em memória */
    int A[MAX_R] = {3, 3}; /* Available inicial (restante zera) */

    /* Max e Allocation por processo (somente posições 0..m-1 são usadas) */
    int Maxs[MAX_P][MAX_R] = {0};   /* zera tudo */
    int Alls[MAX_P][MAX_R] = {0};
    /* P0 */
    Maxs[0][0] = 7; Maxs[0][1] = 5;
    Alls[0][0] = 0; Alls[0][1] = 1;
    /* P1 */
    Maxs[1][0] = 3; Maxs[1][1] = 2;
    Alls[1][0] = 2; Alls[1][1] = 0;

    /* scripts ainda não usados (NULL) */
    struct ReqList *Scripts[MAX_P] = { NULL };

    /* 3) Carrega no sistema */
    sys_load_from_arrays(&s, A, Maxs, Alls, Scripts);

    /* 4) Imprime um resumo do estado carregado */
    puts("\n=== ESTADO APÓS LOAD ===");
    print_vec("Available", s.Available, s.m);

    for (int i = 0; i < s.n; ++i) {
        Process *p = &s.procs[i];
        printf("P%d state=%s\n", p->id, pstate_str(p->state));
        print_vec("  Max       ", p->Max, s.m);
        print_vec("  Allocation", p->Allocation, s.m);
        print_vec("  Need      ", p->Need, s.m);
        printf("  invariants: %s\n", proc_invariants_ok(p) ? "OK" : "FAIL");
    }

    /* 5) Finaliza */
    sim_finalize(&s);
    puts("\n[finalize] ok");
    return 0;
}
