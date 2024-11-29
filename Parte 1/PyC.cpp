#include <iostream>
#include <thread>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <fstream>
#include <unistd.h>
#include "ColaMonitor.h"
#include "PyC.h"

void productor(ColaMonitor &cola, int id, int num_items) {
    for (int i = 0; i < num_items; ++i) {
        cola.agregar(i + id * 1000);
    }
}

void consumidor(ColaMonitor &cola, std::atomic<bool> &producido, int tiempo_espera) {
    while (producido || !cola.esta_vacia()) {
        if (!producido && cola.esta_vacia()) {
            std::this_thread::sleep_for(std::chrono::seconds(tiempo_espera));
            break;
        }
        cola.extraer();
    }
}