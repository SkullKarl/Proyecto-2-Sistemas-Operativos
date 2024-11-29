#ifndef COLAMONITOR_H
#define COLAMONITOR_H



#include <vector>
#include <condition_variable>
#include <mutex>
#include <fstream>

class ColaMonitor {
private:
    std::vector<int> cola;
    size_t capacidad;
    size_t frente, fondo, tam_actual;
    std::mutex mtx;
    std::condition_variable cv_produce, cv_consume;
    std::ofstream log;
    void resize(size_t nueva_capacidad);

public:
    explicit ColaMonitor(size_t tam_inicial);
    ~ColaMonitor();
    void agregar(int item);
    int extraer();
    bool esta_vacia();
};



#endif //COLAMONITOR_H