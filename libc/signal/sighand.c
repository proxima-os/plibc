#include "signal.p.h"

void (*sighand[NUM_SIGNALS])(int);
