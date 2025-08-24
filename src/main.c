/* ---------------------------------------------------------------------
 * main.c — CLI simples: roda cenários e gera logs/metrics
 * --------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "simulator.h"
#include "process.h"
#include "logger.h"

/* loaders (iguais aos que você já usou) */
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
    /* Max alinhado com claim (alloc+script) p/ BANKER não bloquear à toa */
    int Maxs[MAX_P][MAX_R] = { {3,2}, {2,2} };
    int Alls[MAX_P][MAX_R] = { {0,1}, {2,0} };
    struct ReqList *Scripts[MAX_P] = { &r0, &r1 };
    sys_load_from_arrays(S, A, Maxs, Alls, Scripts);
}

/* ----------------- CENÁRIO MÉDIO (n=6, m=3) -----------------
   Ideia: há recursos suficientes para progresso, mas não para
   tudo “de uma vez”. O BANKER concede alguns pedidos e bloqueia
   outros até liberar recursos, rodando safety várias vezes.

   Regras: Max = Allocation_inicial + soma(das reqs do script)
   para manter a semântica do Banqueiro.
---------------------------------------------------------------- */
static void load_medium(System *S) {
    /* n e m esperados: 6 processos, 3 recursos */
    if (S->n != 6 || S->m != 3) {
        fprintf(stderr, "[load_medium] esperado n=6 m=3; recebi n=%d m=%d\n", S->n, S->m);
    }

    static ReqList r[6];
    for (int i = 0; i < 6; ++i) reqlist_init(&r[i]);

    /* Available inicial (ajustado para criar contenção moderada) */
    int A[MAX_R] = {2, 2, 1};

    /* Zere tudo e preencha só 6x3 */
    int Maxs[MAX_P][MAX_R] = {0};
    int Alls[MAX_P][MAX_R] = {0};

    /* ----- P0 ----- */
    Alls[0][0] = 1; Alls[0][1] = 0; Alls[0][2] = 0;
    int p0a[MAX_R] = {1,0,1};
    int p0b[MAX_R] = {0,1,0};
    (void)reqlist_push(&r[0], p0a, S->m);
    (void)reqlist_push(&r[0], p0b, S->m);
    /* Max = Alloc + sum(script) = [1,0,0] + [1,0,1] + [0,1,0] = [2,1,1] */
    Maxs[0][0]=2; Maxs[0][1]=1; Maxs[0][2]=1;

    /* ----- P1 ----- */
    Alls[1][0] = 0; Alls[1][1] = 1; Alls[1][2] = 0;
    int p1a[MAX_R] = {1,0,0};
    int p1b[MAX_R] = {0,0,1};
    (void)reqlist_push(&r[1], p1a, S->m);
    (void)reqlist_push(&r[1], p1b, S->m);
    /* Max = [0,1,0] + [1,0,0] + [0,0,1] = [1,1,1] */
    Maxs[1][0]=1; Maxs[1][1]=1; Maxs[1][2]=1;

    /* ----- P2 ----- */
    Alls[2][0] = 0; Alls[2][1] = 0; Alls[2][2] = 1;
    int p2a[MAX_R] = {1,1,0};
    (void)reqlist_push(&r[2], p2a, S->m);
    /* Max = [0,0,1] + [1,1,0] = [1,1,1] */
    Maxs[2][0]=1; Maxs[2][1]=1; Maxs[2][2]=1;

    /* ----- P3 ----- */
    Alls[3][0] = 1; Alls[3][1] = 0; Alls[3][2] = 1;
    int p3a[MAX_R] = {0,1,0};
    int p3b[MAX_R] = {0,0,1};
    (void)reqlist_push(&r[3], p3a, S->m);
    (void)reqlist_push(&r[3], p3b, S->m);
    /* Max = [1,0,1] + [0,1,0] + [0,0,1] = [1,1,2] */
    Maxs[3][0]=1; Maxs[3][1]=1; Maxs[3][2]=2;

    /* ----- P4 ----- */
    Alls[4][0] = 0; Alls[4][1] = 1; Alls[4][2] = 1;
    int p4a[MAX_R] = {1,0,0};
    (void)reqlist_push(&r[4], p4a, S->m);
    /* Max = [0,1,1] + [1,0,0] = [1,1,1] */
    Maxs[4][0]=1; Maxs[4][1]=1; Maxs[4][2]=1;

    /* ----- P5 ----- */
    Alls[5][0] = 0; Alls[5][1] = 0; Alls[5][2] = 0;
    int p5a[MAX_R] = {1,0,0};
    int p5b[MAX_R] = {0,1,0};
    int p5c[MAX_R] = {0,0,1};
    (void)reqlist_push(&r[5], p5a, S->m);
    (void)reqlist_push(&r[5], p5b, S->m);
    (void)reqlist_push(&r[5], p5c, S->m);
    /* Max = [0,0,0] + [1,0,0]+[0,1,0]+[0,0,1] = [1,1,1] */
    Maxs[5][0]=1; Maxs[5][1]=1; Maxs[5][2]=1;

    struct ReqList *Scripts[MAX_P] = {
        &r[0], &r[1], &r[2], &r[3], &r[4], &r[5]
    };

    sys_load_from_arrays(S, A, Maxs, Alls, Scripts);
}


static void load_deadlock(System *S) {
    static ReqList r0, r1;
    reqlist_init(&r0); reqlist_init(&r1);
    int p0a[MAX_R] = {0,1};
    int p1a[MAX_R] = {1,0};
    (void)reqlist_push(&r0, p0a, S->m);
    (void)reqlist_push(&r1, p1a, S->m);

    int A[MAX_R] = {0,0};
    int Maxs[MAX_P][MAX_R] = { {1,1}, {1,1} };
    int Alls[MAX_P][MAX_R] = { {1,0}, {0,1} };
    struct ReqList *Scripts[MAX_P] = { &r0, &r1 };
    sys_load_from_arrays(S, A, Maxs, Alls, Scripts);
}

static const char *mode_names[] = { "ostrich", "banker", NULL };
static int mode_from_str(const char *s) {
    if (!s) return MODE_OSTRICH;
    if (strcmp(s, "banker") == 0)  return MODE_BANKER;
    if (strcmp(s, "ostrich") == 0) return MODE_OSTRICH;
    return MODE_OSTRICH;
}

static void usage(const char *prog) {
    fprintf(stderr,
    "Uso: %s [--mode ostrich|banker] [--scenario tiny|deadlock|medium]\n"
    "          [--log eventos.csv] [--metrics resumo.json]\n", prog);
}

int main(int argc, char **argv) {
    const char *scenario = "tiny";
    const char *mode_s   = "ostrich";
    const char *csv_path = NULL;
    const char *json_path= NULL;

    static struct option opts[] = {
        {"mode",     required_argument, 0, 'm'},
        {"scenario", required_argument, 0, 's'},
        {"log",      required_argument, 0, 'l'},
        {"metrics",  required_argument, 0, 'j'},
        {"help",     no_argument,       0, 'h'},
        {0,0,0,0}
    };
    int c, idx=0;
    while ((c = getopt_long(argc, argv, "m:s:l:j:h", opts, &idx)) != -1) {
        switch (c) {
            case 'm': mode_s = optarg; break;
            case 's': scenario = optarg; break;
            case 'l': csv_path = optarg; break;
            case 'j': json_path = optarg; break;
            case 'h': default: usage(argv[0]); return (c=='h'?0:1);
        }
    }

    Mode mode = (Mode)mode_from_str(mode_s);

    int n = 2, m = 2;
    if (strcmp(scenario, "tiny") == 0) {
        n = 2; m = 2;
    } else if (strcmp(scenario, "deadlock") == 0) {
        n = 2; m = 2;
    } else if (strcmp(scenario, "medium") == 0) {
        n = 6; m = 3;
    } else {
        fprintf(stderr, "Cenário desconhecido: %s\n", scenario);
        return 2;
    }

    System S;
    sim_init(&S, n, m, mode);

    /* escolhe loader */
    if (strcmp(scenario, "tiny") == 0) {
        load_tiny(&S);
    } else if (strcmp(scenario, "deadlock") == 0) {
        load_deadlock(&S);
    } else { /* medium */
        load_medium(&S);
    }

    /* abre log CSV (se pedido) */
    if (csv_path) {
        if (!logger_open_csv(csv_path, S.m)) {
            fprintf(stderr, "Falha ao abrir CSV: %s\n", csv_path);
        }
    }

    /* roda simulação */
    sim_run(&S);

    /* escreve métricas (se pedido) */
    if (json_path) {
        if (!metrics_write_json(&S, json_path)) {
            fprintf(stderr, "Falha ao escrever JSON: %s\n", json_path);
        }
    }

    /* fecha log */
    logger_close_csv();

    /* imprime resumo no stdout */
    printf("mode=%s scenario=%s | total=%llu grants=%llu blocks=%llu",
           (mode==MODE_BANKER?"BANKER":"OSTRICH"), scenario,
           (unsigned long long)S.metrics.total_requests,
           (unsigned long long)S.metrics.grants,
           (unsigned long long)S.metrics.blocks);
    if (mode == MODE_BANKER) {
        unsigned long long calls = S.metrics.banker_safety_calls;
        unsigned long long ns    = S.metrics.ns_in_safety_total;
        printf(" | safety_calls=%llu ns_total=%llu",
               (unsigned long long)calls, (unsigned long long)ns);
        if (calls) printf(" avg_ns=%llu", (unsigned long long)(ns/calls));
    } else {
        printf(" | deadlocks=%llu t_first=%llu",
               (unsigned long long)S.metrics.deadlocks_found,
               (unsigned long long)S.metrics.time_to_first_deadlock);
    }
    puts("");

    sim_finalize(&S);
    return 0;
}
