/* ---------------------------------------------------------------------
 * logger.c — CSV de eventos + export de métricas em JSON
 * --------------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include "logger.h"

static FILE *g_csv = NULL;
static int   g_m   = 0;

static const char* mode_str(Mode m) {
    return (m == MODE_BANKER) ? "BANKER" : "OSTRICH";
}

bool logger_open_csv(const char *path, int m) {
    if (!path) return false;
    g_csv = fopen(path, "w");
    if (!g_csv) return false;
    if (m < 0) m = 0;
    if (m > MAX_R) m = MAX_R;
    g_m = m;

    /* header */
    fprintf(g_csv, "clock,pid,mode,granted");
    for (int j = 0; j < g_m; ++j) fprintf(g_csv, ",req%d", j);
    for (int j = 0; j < g_m; ++j) fprintf(g_csv, ",avail%d", j);
    fputc('\n', g_csv);
    fflush(g_csv);
    return true;
}

void logger_log_request(const System *S, const Process *P,
                        const int req[MAX_R], bool granted)
{
    if (!g_csv || !S || !P || !req) return;
    fprintf(g_csv, "%llu,%d,%s,%d",
            (unsigned long long)S->sim_clock, P->id, mode_str(S->mode),
            granted ? 1 : 0);

    for (int j = 0; j < g_m; ++j) fprintf(g_csv, ",%d", req[j]);
    for (int j = 0; j < g_m; ++j) fprintf(g_csv, ",%d", S->Available[j]);
    fputc('\n', g_csv);
    /* flush leve para não perder dados em testes curtos */
    fflush(g_csv);
}

void logger_close_csv(void) {
    if (g_csv) {
        fclose(g_csv);
        g_csv = NULL;
        g_m = 0;
    }
}

bool metrics_write_json(const System *S, const char *path) {
    if (!S || !path) return false;
    FILE *f = fopen(path, "w");
    if (!f) return false;

    fprintf(f,
        "{\n"
        "  \"mode\": \"%s\",\n"
        "  \"n\": %d,\n"
        "  \"m\": %d,\n"
        "  \"total_requests\": %llu,\n"
        "  \"grants\": %llu,\n"
        "  \"blocks\": %llu,\n"
        "  \"banker_safety_calls\": %llu,\n"
        "  \"ns_in_safety_total\": %llu,\n"
        "  \"deadlocks_found\": %llu,\n"
        "  \"time_to_first_deadlock\": %llu\n"
        "}\n",
        (S->mode == MODE_BANKER) ? "BANKER" : "OSTRICH",
        S->n, S->m,
        (unsigned long long)S->metrics.total_requests,
        (unsigned long long)S->metrics.grants,
        (unsigned long long)S->metrics.blocks,
        (unsigned long long)S->metrics.banker_safety_calls,
        (unsigned long long)S->metrics.ns_in_safety_total,
        (unsigned long long)S->metrics.deadlocks_found,
        (unsigned long long)S->metrics.time_to_first_deadlock
    );

    fclose(f);
    return true;
}
