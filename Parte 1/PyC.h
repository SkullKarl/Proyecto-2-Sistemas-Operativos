#ifndef PYC_H
#define PYC_H

#include <iostream>
#include <thread>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <fstream>
#include <unistd.h>
#include "ColaMonitor.h"

void productor(ColaMonitor &cola, int id, int num_items);
void consumidor(ColaMonitor &cola, std::atomic<bool> &producido, int tiempo_espera);

#endif //PyC_H