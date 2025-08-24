/* ---------------------------------------------------------------------
 * src/main.c — Comparação Ostrich vs Banker (TINY e DEADLOCK)
 * --------------------------------------------------------------------- */
#include <stdio.h>
#include "simulator.h"
#include "process.h"
#include "banker.h"

static const char* mode_str(Mode m) { return m==MODE_BANKER ? "BANKER" : "OSTRICH"; }
static void print_vec(const char *label, const int *v, int m) {
    printf("%s=[", label); for (int j=0;j<m;++j) printf("%d%s", v[j], (j+1<m)?",":""); puts("]");
}
static void print_proc(const System *S, const Process *p) {
    printf("P%d state=FINISHED? %s\n", p->id, (p->state==P_FINISHED?"yes":"no"));
    print_vec("  Max       ", p->Max, S->m);
    print_vec("  Allocation", p->Allocation, S->m);
    print_vec("  Need      ", p->Need, S->m);
}
static void print_metrics(const System *S) {
    printf("  total_requests=%llu, grants=%llu, blocks=%llu\n",
           (unsigned long long)S->metrics.total_requests,
           (unsigned long long)S->metrics.grants,
           (unsigned long long)S->metrics.blocks);
    if (S->mode == MODE_BANKER) {
        printf("  banker_safety_calls=%llu, ns_in_safety_total=%llu",
               (unsigned long long)S->metrics.banker_safety_calls,
               (unsigned long long)S->metrics.ns_in_safety_total);
        if (S->metrics.banker_safety_calls) {
            unsigned long long avg = S->metrics.ns_in_safety_total / S->metrics.banker_safety_calls;
            printf(", avg_safety_ns=%llu", (unsigned long long)avg);
        }
        puts("");
    } else {
        printf("  deadlocks_found=%llu, time_to_first_deadlock=%llu\n",
               (unsigned long long)S->metrics.deadlocks_found,
               (unsigned long long)S->metrics.time_to_first_deadlock);
    }
}

/* ------- CENÁRIOS ------- */
static void load_tiny(System *S) {
    static ReqList r0, r1;
    reqlist_init(&r0); reqlist_init(&r1);
    int req0a[MAX_R] = {1,0};
    int req0b[MAX_R] = {2,1};
    int req1a[MAX_R] = {0,2};
    (void)reqlist_push(&r0, req0a, S->m);
    (void)reqlist_push(&r0, req0b, S->m);
    (void)reqlist_push(&r1, req1a, S->m);

    int A[MAX_R] = {3,3};
    int Maxs[MAX_P][MAX_R] = { {3,2}, {2,2} };
    int Alls[MAX_P][MAX_R] = { {0,1}, {2,0} };
    struct ReqList *Scripts[MAX_P] = { &r0, &r1 };
    sys_load_from_arrays(S, A, Maxs, Alls, Scripts);
}

static void load_deadlock(System *S) {
    /* Clássico: deve deadlock no Ostrich, evitar no Banker */
    static ReqList r0, r1;
    reqlist_init(&r0); reqlist_init(&r1);
    int p0a[MAX_R] = {1,0}; /* depois ficará precisando (0,1) */
    int p0b[MAX_R] = {0,1};
    int p1a[MAX_R] = {0,1}; /* depois ficará precisando (1,0) */
    int p1b[MAX_R] = {1,0};
    (void)reqlist_push(&r0, p0a, S->m);
    (void)reqlist_push(&r0, p0b, S->m);
    (void)reqlist_push(&r1, p1a, S->m);
    (void)reqlist_push(&r1, p1b, S->m);

    int A[MAX_R] = {0,0};
    int Maxs[MAX_P][MAX_R] = { {1,1}, {1,1} };
    int Alls[MAX_P][MAX_R] = { {1,0}, {0,1} };
    struct ReqList *Scripts[MAX_P] = { &r0, &r1 };
    sys_load_from_arrays(S, A, Maxs, Alls, Scripts);
}

/* ------- Runner genérico ------- */
typedef void (*LoaderFn)(System*);
static void run_scenario(const char *name, Mode mode, LoaderFn loader) {
    System S;
    sim_init(&S, 2, 2, mode);
    loader(&S);

    printf("\n=== RUN %s | mode=%s ===\n", name, mode_str(mode));
    print_vec("Available(start)", S.Available, S.m);

    sim_run(&S);

    printf("invariants pós-run: %s\n", sys_invariants_ok(&S) ? "OK" : "FAIL");
    print_metrics(&S);

    print_vec("Available(final)", S.Available, S.m);
    for (int i = 0; i < S.n; ++i) print_proc(&S, &S.procs[i]);

    sim_finalize(&S);
}

int main(void) {
    /* TINY: deve terminar nos dois modos */
    run_scenario("TINY", MODE_OSTRICH, load_tiny);
    run_scenario("TINY", MODE_BANKER,  load_tiny);

    /* DEADLOCK: Ostrich deve detectar deadlock; Banker deve evitar (bloquear) */
    run_scenario("DEADLOCK", MODE_OSTRICH, load_deadlock);
    run_scenario("DEADLOCK", MODE_BANKER,  load_deadlock);
    return 0;
}
