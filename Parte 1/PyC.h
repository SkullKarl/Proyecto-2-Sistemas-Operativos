#ifndef PYC_H
#define PYC_H

#include <fstream>
#include "ColaMonitor.h"

void productor(ColaMonitor &cola, int id, int num_items);
void consumidor(ColaMonitor &cola, std::atomic<bool> &producido, int tiempo_espera);

#endif //PyC_H