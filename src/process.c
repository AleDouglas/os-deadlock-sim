/* ---------------------------------------------------------------------
 * src/process.c
 * Stubs das rotinas de processo (base sem lógica).
 * --------------------------------------------------------------------- */

#include "process.h"

/*
 * proc_reset
 * Inicialmente vazio: será responsável por zerar campos e definir state = P_NEW.
 * Serve para “zerar” um Process e deixá-lo num estado inicial consistente antes de qualquer carga de cenário
 */
void proc_reset(Process *p, int id) {
    if (p == NULL) return;
    p->id = id;
    p->state = P_NEW;
    for (int j = 0; j < MAX_R-1; j++){
        p->Max[j] = 0;
        p->Allocation[j] = 0;
        p->Need[j] = 0;
    }
    p->script = NULL;  /* ainda não implementado */
    p->wait_time_acc = 0;
}

/*
 * proc_compute_need
 * Inicialmente vazio: será responsável por Need[j] = Max[j] - Allocation[j].
 * Basicamente, é o cálculo de necessidade de recursos.
 */
void proc_compute_need(Process *p) {
    if ( p == NULL ) return;
    for (int j = 0; j < MAX_R-1; j++) {
        p->Need[j] = p->Max[j] - p->Allocation[j];
    }
}

/*
 * proc_invariants_ok
 * Inicialmente retorna true: depois validará Need = Max - Allocation e não-negatividade.
 */
bool proc_invariants_ok(const Process *p) {
    if (p == NULL) return false;
    for (int j = 0; j < MAX_R-1; j++) {
        if (p->Need[j] != p->Max[j] - p->Allocation[j]) return false;
        if (p->Need[j] < 0) return false;
    }
    return true;
}
