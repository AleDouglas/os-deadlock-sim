/*
* Centralizar constantes globais (limites como MAX_P, MAX_R);
* Definir tipos básicos (apelidos de inteiros);
* Declarar os enums fundamentais: Mode (banker/ostrich) e PState (ciclo de vida do processo);
* Garantir que todo o resto do projeto possa incluir isso sem dependências cíclicas
*/

#ifndef RESOURCES_H
#define RESOURCES_H

/* ---------------------------------------------------------------------
 * resources.h
 * Base de tipos e constantes globais do simulador.
 * --------------------------------------------------------------------- */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* ===========================================================
 * Limites globais (ajuste conforme necessário)
 * =========================================================== */
#define MAX_REQS 64     /* número máximo de requisições por processo */
#define MAX_P 1024      /* número máximo de processos suportados */
#define MAX_R   32      /* número máximo de tipos de recurso     */
/* Validações de compilação (evita valores inválidos) */
_Static_assert(MAX_P > 0, "MAX_P deve ser > 0");
_Static_assert(MAX_R > 0, "MAX_R deve ser > 0");
_Static_assert(MAX_REQS > 0, "MAX_REQS deve ser > 0");

/* ===========================================================
 * Apelidos de inteiros (qualidade de vida)
 * =========================================================== */
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/* ===========================================================
 * Compatibilidade C++
 * =========================================================== */
#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================================
 * Modo de execução do simulador
 * =========================================================== */
typedef enum Mode {
    MODE_BANKER = 0,   /* Evita deadlock (algoritmo do banqueiro) */
    MODE_OSTRICH       /* Ignora prevenção; detector só para métricas */
} Mode;

/* ===========================================================
 * Estado do processo (ciclo de vida no simulador)
 * =========================================================== */
typedef enum PState {
    P_NEW = 0,     /* criado, ainda não pronto */
    P_READY,       /* pronto para executar (na fila READY) */
    P_RUNNING,     /* em execução (despachado pelo simulador) */
    P_BLOCKED,     /* aguardando recursos */
    P_FINISHED     /* terminou; recursos liberados */
} PState;

/* (Opcional) macro utilitária para tamanho de array estático */
#ifndef ARRAY_LEN
#define ARRAY_LEN(a) ((int)(sizeof(a)/sizeof((a)[0])))
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RESOURCES_H */