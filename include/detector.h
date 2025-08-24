#ifndef DETECTOR_H
#define DETECTOR_H
/* ---------------------------------------------------------------------
 * detector.h — Detector de deadlock (para métricas no modo OSTRICH)
 * --------------------------------------------------------------------- */
#include <stdbool.h>
#include "resources.h"
#include "simulator.h"

#ifdef __cplusplus
extern "C" {
#endif

bool detect_deadlock(const System *S);

#ifdef __cplusplus
}
#endif
#endif /* DETECTOR_H */
