/* ---------------------------------------------------------------------
 * src/main.c
 * Driver mínimo de teste: verifica link/headers e chamadas básicas.
 * --------------------------------------------------------------------- */

#include <stdio.h>
#include "simulator.h"

/* Helper simples para imprimir o modo em texto */
static const char* mode_str(Mode m) {
    return (m == MODE_BANKER) ? "BANKER" : "OSTRICH";
}

int main(void) {
    System s;

    /* Inicializa o sistema com 3 processos, 2 recursos, modo BANQUEIRO */
    sim_init(&s, 3, 2, MODE_BANKER);

    /* Prints básicos: confirmam que o init executou e campos foram setados */
    printf("[init] n=%d, m=%d, mode=%s\n", s.n, s.m, mode_str(s.mode));

    /* (Opcional) se já implementou, sim_clock deve começar em 0 */
    printf("[init] sim_clock=%llu\n", (unsigned long long)s.sim_clock);

    /* Reset preserva n/m/mode, zera clock e métricas (se você implementou) */
    sim_reset(&s);
    printf("[reset] n=%d, m=%d, mode=%s, sim_clock=%llu\n",
           s.n, s.m, mode_str(s.mode),
           (unsigned long long)s.sim_clock);

    /* Finalização (por enquanto, no-op) */
    sim_finalize(&s);
    printf("[finalize] ok\n");

    return 0;
}
