#include "ColaMonitor.h"

ColaMonitor::ColaMonitor(size_t tam_inicial)
    : capacidad(tam_inicial), frente(0), fondo(0), tam_actual(0) {
    cola.resize(capacidad);
    log.open("log.txt", std::ios::out);
    if (log.is_open()) {
        log << "Inicializando la cola con tamaño " << tam_inicial << ".\n";
    }
}

ColaMonitor::~ColaMonitor() {
    if (log.is_open()) {
        log.close();
    }
}

void ColaMonitor::resize(size_t nueva_capacidad) {
    std::vector<int> nueva_cola(nueva_capacidad);
    for (size_t i = 0; i < tam_actual; ++i) {
        nueva_cola[i] = cola[(frente + i) % capacidad];
    }
    cola = std::move(nueva_cola);
    frente = 0;
    fondo = tam_actual;
    capacidad = nueva_capacidad;
    if (log.is_open()) {
        log << "Cambio de tamaño de la cola a " << nueva_capacidad << " elementos.\n";
    }
}

void ColaMonitor::agregar(int item) {
    std::unique_lock<std::mutex> lock(mtx);
    cv_produce.wait(lock, [this]() { return tam_actual < capacidad; });
    cola[fondo] = item;
    fondo = (fondo + 1) % capacidad;
    tam_actual++;
    if (tam_actual == capacidad) {
        resize(capacidad * 2);
    }
    if (log.is_open()) {
        log << "Elemento producido: " << item << ". Tamaño actual: " << tam_actual << ".\n";
    }
    cv_consume.notify_one();
}

int ColaMonitor::extraer() {
    std::unique_lock<std::mutex> lock(mtx);
    cv_consume.wait(lock, [this]() { return tam_actual > 0; });
    int item = cola[frente];
    frente = (frente + 1) % capacidad;
    tam_actual--;
    if (tam_actual > 0 && tam_actual <= capacidad / 4) {
        resize(capacidad / 2);
    }
    if (log.is_open()) {
        log << "Elemento consumido: " << item << ". Tamaño actual: " << tam_actual << ".\n";
    }
    cv_produce.notify_one();
    return item;
}

bool ColaMonitor::esta_vacia() {
    std::lock_guard<std::mutex> lock(mtx);
    return tam_actual == 0;
}