#ifndef LOGGER_H
#define LOGGER_H
/* ---------------------------------------------------------------------
 * logger.h — CSV de eventos de requisição + export de métricas (JSON)
 * --------------------------------------------------------------------- */
#include <stdbool.h>
#include "resources.h"
#include "simulator.h"
#include "process.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Abre CSV e escreve header (usa m colunas de req e available) */
bool logger_open_csv(const char *path, int m);

/* Registra um evento de requisição (após decisão, para ter estado atualizado).
   Campos: clock,pid,mode,granted,req[0..m-1],avail[0..m-1] */
void logger_log_request(const System *S, const Process *P,
                        const int req[MAX_R], bool granted);

/* Fecha arquivo CSV (se aberto) */
void logger_close_csv(void);

/* Exporta métricas (JSON) para ‘path’ */
bool metrics_write_json(const System *S, const char *path);

#ifdef __cplusplus
}
#endif
#endif /* LOGGER_H */
