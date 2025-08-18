/*
 * src/simulator.c
 * Simulador do sistema de gerenciamento de processos.
 */

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


void sim_finalize(System *s) {
    if (s == NULL) return;
}
