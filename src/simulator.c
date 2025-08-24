/*
 * src/simulator.c
 * Simulador do sistema de gerenciamento de processos.
 */
#include <assert.h>
#include "simulator.h"
#include "process.h"
#include "detector.h"

/*
 * sim_init
 * Inicializa o simulador: preparar o sistema do zero, com contadores zerados e processos resetados.
 */
void sim_init(System *s, int n, int m, Mode mode) {
    if (s == NULL) return;
    s->n = n;
    s->m = m;
    s->mode = mode;
    s->sim_clock = 0;

    metrics_reset(&s->metrics);

    for (int j = 0; j < MAX_R; j++) {
        s->Available[j] = 0;
    }

    // Inicializando todos os processos
    for (int i = 0; i < MAX_P; i++) {
        proc_reset(&s->procs[i], i);
    }
}

/*
 * sim_reset
 * Limpa os recursos utilizados pelo simulador: limpar estado de execução preservando a configuração (n, m, mode).
 */
void sim_reset(System *s) {
    if ( s == NULL ) return;
    s->sim_clock = 0;

    metrics_reset(&s->metrics);
    for (int j = 0; j < MAX_R; j++) {
        s->Available[j] = 0;
    }

    for (int i = 0; i < MAX_P; i++) {
        proc_reset(&s->procs[i], i);
    }
}

/*
 * sim_finalize
 * Limpa os recursos utilizados pelo simulador: limpar estado de execução preservando a configuração (n, m, mode).
 */
void sim_finalize(System *s) {
    if (s == NULL) return;
}

/*
 * sys_invariants_ok
 * Garantir que o sistema está coerente antes de simular qualquer coisa.
 */
bool sys_invariants_ok(const System *s){
    if (s == NULL) return false;
    if (s->n < 1 || s->n > MAX_P) return false;
    if (s->m < 1 || s->m > MAX_R) return false;

    // Available não-negativo
    for (int j = 0; j < s->m; j++) {
        if (s->Available[j] < 0) return false;
    }

    for (int j = s->m; j < MAX_R; j++) {
        if (s->Available[j] != 0) return false;
    }
    
    // checar processos:
    for (int i = 0; i < s->n; i++) {
        if (!proc_invariants_ok(&s->procs[i])) return false;
        // (opcional) garantir zeros fora de m
        for (int j = s->m; j < MAX_R; j++) {
            if (s->procs[i].Max[j] != 0) return false;
            if (s->procs[i].Allocation[j] != 0) return false;
            if (s->procs[i].Need[j] != 0) return false;
        }
    }

    return true;
}

/*
 * Carrega um cenário diretamente de arrays (sem ler arquivo).
 * - available0[j]  : instâncias livres iniciais por recurso (0..m-1)
 * - maxs[i][j]     : demanda máxima do processo i para o recurso j
 * - allocs[i][j]   : alocação inicial do processo i para o recurso j
 * - scripts[i]     : ponteiro para a lista de requisições do processo i (pode ser NULL)
 *
 * Observações:
 * - Zera qualquer posição acima de m em Available/Max/Allocation.
 * - Recalcula Need = Max - Allocation.
 * - Coloca processos 0..n-1 em P_READY (prontos para executar).
 * - Processos acima de n são resetados para um estado neutro.
 */
void sys_load_from_arrays(System *s,
                          const int available0[MAX_R],
                          const int maxs[MAX_P][MAX_R],
                          const int allocs[MAX_P][MAX_R],
                          struct ReqList *scripts[MAX_P])
{
    assert(s != NULL);
    /* n e m devem ter sido definidos antes em sim_init */
    assert(s->n >= 0 && s->n <= MAX_P);
    assert(s->m > 0  && s->m <= MAX_R);

    /* ---- Available ---- */
    for (int j = 0; j < s->m; ++j) {
        s->Available[j] = (available0 ? available0[j] : 0);
    }

    for (int j = s->m; j < MAX_R; ++j) {
        s->Available[j] = 0;
    }

    /* ---- Processos ativos 0..n-1 ---- */
    for (int i = 0; i < s->n; ++i) {
        Process *p = &s->procs[i];

        /* Base limpa */
        proc_reset(p, i);

        /* Copiar Max/Allocation até m-1; manter zeros no restante */
        for (int j = 0; j < s->m; ++j) {
            p->Max[j]        = maxs   ? maxs[i][j]   : 0;
            p->Allocation[j] = allocs ? allocs[i][j] : 0;
        }
        for (int j = s->m; j < MAX_R; ++j) {
            p->Max[j]        = 0;
            p->Allocation[j] = 0;
        }

        /* Need = Max - Allocation */
        proc_compute_need(p);

        /* Script (pode ser NULL por enquanto) */
        p->script = scripts ? scripts[i] : NULL;

        /* Pronto para execução */
        p->state = P_READY;
    }

    /* ---- Processos inativos n..MAX_P-1 (mantém estado neutro) ---- */
    for (int i = s->n; i < MAX_P; ++i) {
        proc_reset(&s->procs[i], i);
        /* opcional: marcar explicitamente como “finalizados”
           s->procs[i].state = P_FINISHED; */
    }

    assert(sys_invariants_ok(s) && "invariantes globais violadas apos load");
}


/* Liberação simplificada (essa já é útil de verdade) */
void release_all_resources(System *S, Process *P) {
    if (!S || !P) return;
    for (int j = 0; j < S->m; ++j) {
        S->Available[j] += P->Allocation[j];
        P->Allocation[j] = 0;
        P->Need[j]       = 0;   /* ou: recompute depois com proc_compute_need(P) */
        P->Max[j]        = 0;
    }
}

bool handle_request_current_mode(System *S, Process *P, const int req[MAX_R]);
void release_all_resources(System *S, Process *P);
void enqueue_ready(int pid);
void enqueue_blocked(int pid);
int  blocked_count(void);

/*
 * Processa um processo por um "passo" de simulação.
 * Retorna true se o processo TERMINOU neste passo.
 */
bool sim_step_handle_process(System *S, Process *p) {
    int req[MAX_R] = {0};

    /* 1) Sem roteiro → termina e libera tudo */
    if (p->script == NULL || reqlist_empty(p->script)) {
        release_all_resources(S, p);
        p->state = P_FINISHED;
        return true;
    }

    /* 2) Lê a próxima requisição sem consumir */
    if (!reqlist_peek(p->script, req)) {
        /* Roteiro inconsistente: trate como fim */
        release_all_resources(S, p);
        p->state = P_FINISHED;
        return true;
    }

    /* 3) Pede concessão conforme o modo atual (Ostrich/Banker) */
    bool granted = handle_request_current_mode(S, p, req);

    if (granted) {
        /* 4a) Avança o roteiro */
        (void)reqlist_pop(p->script);

        /* 4b) Se acabou o roteiro, termina liberando tudo */
        if (reqlist_empty(p->script)) {
            release_all_resources(S, p);
            p->state = P_FINISHED;
            return true;
        }

        /* 4c) Ainda tem requisições → volta para READY */
        p->state = P_READY;
        enqueue_ready(p->id);
        return false;
    } else {
        /* 4d) Não concedido → vai para BLOCKED */
        p->state = P_BLOCKED;
        enqueue_blocked(p->id);
        (void)blocked_count();  /* útil para métricas/log, se quiser */
        return false;
    }
}

/* ------------------------------------------------------------- */
/* Varre todos os processos BLOQUEADOS e tenta a MESMA req de novo.
 * Retorna true se pelo menos um processo mudou de estado (desbloqueou
 * ou terminou).
 * ------------------------------------------------------------- */
static bool sweep_blocked(System *S) {
    bool progress = false;
    int  req[MAX_R];

    for (int i = 0; i < S->n; ++i) {
        Process *p = &S->procs[i];
        if (p->state != P_BLOCKED) continue;

        /* Se não há roteiro, finalize por segurança */
        if (p->script == NULL || reqlist_empty(p->script)) {
            release_all_resources(S, p);
            p->state = P_FINISHED;
            progress = true;
            continue;
        }

        /* Tenta novamente a requisição atual */
        if (!reqlist_peek(p->script, req)) {
            /* Roteiro inconsistente → finalize */
            release_all_resources(S, p);
            p->state = P_FINISHED;
            progress = true;
            continue;
        }

        bool granted = handle_request_current_mode(S, p, req);
        if (granted) {
            (void)reqlist_pop(p->script);

            if (reqlist_empty(p->script)) {
                release_all_resources(S, p);
                p->state = P_FINISHED;
            } else {
                p->state = P_READY;
            }
            progress = true;
        }
        /* senão, continua BLOQUEADO */
    }

    return progress;
}

/* ------------------------------------------------------------- */
/* Retorna true se TODOS os processos 0..n-1 estão FINISHED      */
/* ------------------------------------------------------------- */
static bool all_finished(const System *S) {
    for (int i = 0; i < S->n; ++i) {
        if (S->procs[i].state != P_FINISHED) return false;
    }
    return true;
}

/* ------------------------------------------------------------- */
/* Loop de simulação mínimo (sem filas): varre READY, tenta passos,
 * tenta desbloquear bloqueados e avança o relógio.
 * Para evitar loop infinito quando nada muda (tudo bloqueado),
 * paramos se não houver progresso em uma rodada completa.
 * ------------------------------------------------------------- */
void sim_run(System *S) {
    if (!S) return;

    while (!all_finished(S)) {
        bool progress = false;

        /* 1) Passo nos processos READY */
        for (int i = 0; i < S->n; ++i) {
            Process *p = &S->procs[i];
            if (p->state != P_READY) continue;

            /* Snapshot do estado antes */
            PState before = p->state;

            /* sim_step_handle_process deve estar no seu simulator.c */
            extern bool sim_step_handle_process(System *S, Process *p);
            bool finished_now = sim_step_handle_process(S, p);

            if (finished_now || p->state != before) {
                progress = true;
            }
        }

        /* 2) Tentar desbloquear bloqueados */
        if (sweep_blocked(S)) {
            progress = true;
        }

        /* 3) Avança o relógio lógico */
        S->sim_clock += 1;

        /* 4) Se não houve progresso na rodada, paramos (evita loop infinito) */
        if (!progress) {
            if (S->mode == MODE_OSTRICH) {
                bool has_blocked = false;
                for (int i = 0; i < S->n; ++i) {
                    if (S->procs[i].state == P_BLOCKED) { has_blocked = true; break; }
                }
                if (has_blocked && detect_deadlock(S)) {
                    S->metrics.deadlocks_found += 1;
                    if (S->metrics.time_to_first_deadlock == 0) {
                        S->metrics.time_to_first_deadlock = S->sim_clock;
                    }
                }
            }
            break; /* evita loop infinito */
        }
    }
}
