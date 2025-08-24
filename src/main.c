/* ---------------------------------------------------------------------
 * src/main.c
 * Teste integrado: System + ReqList + sys_load_from_arrays + sim_step.
 * --------------------------------------------------------------------- */
#include <stdio.h>
#include "simulator.h"
#include "process.h"   /* ReqList, reqlist_* e Process */

/* sim_step_handle_process foi definido em simulator.c; declara aqui para usar */
extern bool sim_step_handle_process(System *S, Process *p);

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
static void print_proc(const System *S, const Process *p) {
    printf("P%d state=%s\n", p->id, pstate_str(p->state));
    print_vec("  Max       ", p->Max, S->m);
    print_vec("  Allocation", p->Allocation, S->m);
    print_vec("  Need      ", p->Need, S->m);
}

int main(void) {
    /* 1) Inicializa o sistema: 2 processos, 2 recursos, modo OSTRICH */
    System S;
    sim_init(&S, 2, 2, MODE_OSTRICH);
    printf("[init] n=%d m=%d mode=%s clock=%llu\n",
           S.n, S.m, mode_str(S.mode), (unsigned long long)S.sim_clock);

    /* 2) Cria ReqList para cada processo e adiciona requisições */
    ReqList r0, r1;
    reqlist_init(&r0);
    reqlist_init(&r1);

    /* m = S.m; preenche só até m-1; o resto fica zero */
    int req0a[MAX_R] = {1, 0};   /* P0 pede 1 do R0 */
    int req0b[MAX_R] = {2, 1};   /* depois 2 do R0 e 1 do R1 */
    int req1a[MAX_R] = {0, 2};   /* P1 pede 2 do R1 */

    (void)reqlist_push(&r0, req0a, S.m);
    (void)reqlist_push(&r0, req0b, S.m);
    (void)reqlist_push(&r1, req1a, S.m);

    /* 3) Define um cenário tiny em memória */
    int A[MAX_R] = {3, 3}; /* Available inicial */

    int Maxs[MAX_P][MAX_R] = {0};
    int Alls[MAX_P][MAX_R] = {0};
    /* P0 */
    Maxs[0][0] = 7; Maxs[0][1] = 5;
    Alls[0][0] = 0; Alls[0][1] = 1;
    /* P1 */
    Maxs[1][0] = 3; Maxs[1][1] = 2;
    Alls[1][0] = 2; Alls[1][1] = 0;

    struct ReqList *Scripts[MAX_P] = { &r0, &r1 };

    /* 4) Carrega no sistema e checa invariantes */
    sys_load_from_arrays(&S, A, Maxs, Alls, Scripts);



    puts("\n=== ESTADO APÓS LOAD ===");
    print_vec("Available", S.Available, S.m);
    for (int i = 0; i < S.n; ++i) {
        print_proc(&S, &S.procs[i]);
        printf("  reqlist_count=%d empty=%s\n",
               reqlist_count(S.procs[i].script),
               reqlist_empty(S.procs[i].script) ? "yes" : "no");
    }

    printf("\ninvariants: %s\n", sys_invariants_ok(&S) ? "OK" : "FAIL");

    /* 5) Executa alguns “passos” (com o stub atual, deve bloquear) */

    printf("\n=== RODANDO sim_run ===\n");
    sim_run(&S);

    puts("\n=== ESTADO FINAL (após passos) ===");
    for (int i = 0; i < S.n; ++i) {
        print_proc(&S, &S.procs[i]);
    }

    /* 6) Finaliza */
    sim_finalize(&S);
    puts("\n[finalize] ok");
    return 0;
}
