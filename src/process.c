/* ---------------------------------------------------------------------
 * src/process.c
 * Stubs das rotinas de processo (base sem lógica).
 * --------------------------------------------------------------------- */

#include "process.h"


void reqlist_init(ReqList *rl){
    if (rl == NULL) return;
    rl->len = 0;
    rl->idx = 0;
    for (int i = 0; i < MAX_REQS; i++) {
        for (int j = 0; j < MAX_R; j++) {
            rl->items[i][j] = 0;
        }
    }

}

bool reqlist_empty(const ReqList *rl){
    return (rl->idx >= rl->len);
}

int reqlist_count(const ReqList *rl){
    if (rl == NULL) return 0;
    return rl->len - rl->idx;
}

/*
# Adiciona uma requisição ao final (cópia defensiva)
*/
bool reqlist_push(ReqList *rl, const int *req, int m) {
    if (rl == NULL || req == NULL || m <= 0) return false;
    if (rl->len >= MAX_REQS) return false;

    // para j de 0 até m-1:
    for(int j = 0; j < m; j++){
        rl->items[rl->len][j] = req[j];
    }
    // para j de m até MAX_R-1:
    for(int j = m; j < MAX_R; j++){
        rl->items[rl->len][j] = 0;
    }
    rl->len++;
    return true;
}

bool reqlist_peek(const ReqList *rl, int out_req[MAX_R]){
    if (reqlist_empty(rl)) return false;
    int r = rl->idx;
    for(int j = 0; j < MAX_R; j++){
        out_req[j] = rl->items[r][j];
    }
    return true;
}
bool reqlist_pop(ReqList *rl){
    if (rl == NULL || reqlist_empty(rl)) return false;
    rl->idx++;
    return true;
}
void reqlist_rewind(ReqList *rl){
    if (rl == NULL) return;
    rl->idx = 0;
}


/*
 * proc_reset
 * Inicialmente vazio: será responsável por zerar campos e definir state = P_NEW.
 * Serve para “zerar” um Process e deixá-lo num estado inicial consistente antes de qualquer carga de cenário
 */
void proc_reset(Process *p, int id) {
    if (p == NULL) return;
    p->id = id;
    p->state = P_NEW;
    for (int j = 0; j < MAX_R; j++){
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
    for (int j = 0; j < MAX_R; j++) {
        p->Need[j] = p->Max[j] - p->Allocation[j];
    }
}

/*
 * proc_invariants_ok
 * Inicialmente retorna true: depois validará Need = Max - Allocation e não-negatividade.
 */
bool proc_invariants_ok(const Process *p) {
    if (p == NULL) return false;
    for (int j = 0; j < MAX_R; j++) {
        if (p->Need[j] != p->Max[j] - p->Allocation[j]) return false;
        if (p->Need[j] < 0) return false;
    }
    return true;
}
