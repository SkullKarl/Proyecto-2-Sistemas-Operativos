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

int main(int argc, char *argv[]) {
    int num_productores = 1, num_consumidores = 1, tam_inicial = 10, tiempo_espera = 1;
    int opt;
    while ((opt = getopt(argc, argv, "p:c:s:t:")) != -1) {
        switch (opt) {
            case 'p': num_productores = std::stoi(optarg); break;
            case 'c': num_consumidores = std::stoi(optarg); break;
            case 's': tam_inicial = std::stoi(optarg); break;
            case 't': tiempo_espera = std::stoi(optarg); break;
            default:
                std::cerr << "Uso: " << argv[0] << " -p <productores> -c <consumidores> -s <tamaño> -t <tiempo>\n";
                return EXIT_FAILURE;
        }
    }

    ColaMonitor cola(tam_inicial);
    std::atomic<bool> producido(true);
    std::vector<std::thread> productores, consumidores;

    for (int i = 0; i < num_productores; ++i) {
        productores.emplace_back(productor, std::ref(cola), i, 100);
    }
    for (int i = 0; i < num_consumidores; ++i) {
        consumidores.emplace_back(consumidor, std::ref(cola), std::ref(producido), tiempo_espera);
    }

    for (auto &prod : productores) {
        prod.join();
    }
    producido = false;

    for (auto &cons : consumidores) {
        cons.join();
    }

    return 0;
}