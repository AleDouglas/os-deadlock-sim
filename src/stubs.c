/* src/stubs.c — implementações temporárias só para linkar */
#include "resources.h"
#include "process.h"
#include "simulator.h"




/* Filas ainda não existem — stubs no-op */
void enqueue_ready(int pid)   { (void)pid; /* TODO: implementar fila READY */ }
void enqueue_blocked(int pid) { (void)pid; /* TODO: implementar fila BLOCKED */ }
int  blocked_count(void)      { return 0;  /* TODO: retornar tamanho real */ }
