# Proyecto Sistemas Operativos

Este proyecto consta de dos partes un simulador que maneje una cola circular de tamaño dinámico compartida entre múltiples hebras y otro simulador de memoria virtual que permite evaluar diferentes algoritmos de reemplazo de páginas. 

## Características
- Cola circular de tamaño dinámico.
- Múltiples hebras productoras y consumidoras.
- Monitor para manejo de concurrencia.
- Algoritmos de reemplazo de páginas (FIFO, LRU, CLOCK y OPT)
- Conteo de fallos de página

## Compilación
Clona el repositorio en tu máquina local.
```
git clone https://github.com/SkullKarl/Proyecto-2-Sistemas-Operativos.git
```
Navega al directorio del proyecto.
```
cd Proyecto-2-Sistemas-Operativos
```
Para la Parte 1 (cola circular) ir al directorio respectivo y usar g++:

    g++ -std=c++17 -pthread main.cpp ColaMonitor.cpp PyC.cpp -o Parte1



Para ejecutar el programa, simplemente ejecuta el archivo compilado:
```
./Parte1
```
Para la Parte 2 (memoria virtual) ir al directorio respectivo y usar gcc:

    gcc -o mvirtual mvirtual.c



Para ejecutar el programa, simplemente ejecuta el archivo compilado:
```
./mvirtual
```
