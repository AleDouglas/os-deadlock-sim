/* ---------------------------------------------------------------------
 * ostrich.c — Dispatcher do modo OSTRICH (ignora prevenção de deadlock)
 * Concede req se houver Available suficiente e sem ultrapassar Need.
 * --------------------------------------------------------------------- */
#include <stdbool.h>
#include "resources.h"
#include "simulator.h"
#include "process.h"
