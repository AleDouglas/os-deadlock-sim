/*
 * src/simulator.c
 * Simulador do sistema de gerenciamento de processos.
 */
#include <assert.h>
#include "simulator.h"

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

    for (int j = 0; j < MAX_R-1; j++) {
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
    for (int j = 0; j < MAX_R-1; j++) {
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
    for(int j = 0; j < MAX_R-1; j++) {
        if (s->Available[j] < 0) return false;
    }
    
    // processos 0..n-1 devem ser coerentes
    for(int i = 0; i < s->n; i++) {
        Process p = s->procs[i];
        if (!proc_invariants_ok(&p)) return false;
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

    /* Neste ponto, se já tiver implementado sys_invariants_ok,
       você pode ativar a asserção abaixo:
       assert(sys_invariants_ok(s) && "invariantes globais violadas apos load");
    */
}