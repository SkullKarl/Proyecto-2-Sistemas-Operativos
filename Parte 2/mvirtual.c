#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define TABLE_SIZE 101
#define MAX_REFS 1000
#define MAX_FRAMES 100

// Estructuras de datos
typedef struct {
    int page_number;
    int referenced;
    int last_used;
} Page;

typedef struct HashNode {
    int page_number;
    int frame_index;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* table[TABLE_SIZE];
} PageTable;

typedef struct {
    int* queue;
    int front;
    int rear;
    int size;
} FIFOQueue;

// Variables globales para los algoritmos
FIFOQueue fifo_queue;
int clock_hand = 0;
int* referenced;
int clock_ticks = 0;
int* future_refs;
int future_refs_count = 0;

// Funciones para la tabla hash
int hash(int page_number) {
    return page_number % TABLE_SIZE;
}

void init_page_table(PageTable* pt) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        pt->table[i] = NULL;
    }
}

void insert_page(PageTable* pt, int page_number, int frame_index) {
    int index = hash(page_number);
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    new_node->page_number = page_number;
    new_node->frame_index = frame_index;
    new_node->next = pt->table[index];
    pt->table[index] = new_node;
}

int get_frame(PageTable* pt, int page_number) {
    int index = hash(page_number);
    HashNode* current = pt->table[index];
    while (current != NULL) {
        if (current->page_number == page_number) {
            return current->frame_index;
        }
        current = current->next;
    }
    return -1;
}

void update_page_table(PageTable* pt, int page_number, int new_frame) {
    int index = hash(page_number);
    HashNode* current = pt->table[index];
    while (current != NULL) {
        if (current->page_number == page_number) {
            current->frame_index = new_frame;
            return;
        }
        current = current->next;
    }
    insert_page(pt, page_number, new_frame);
}

// Funciones para FIFO
void init_fifo(int size) {
    fifo_queue.queue = (int*)malloc(size * sizeof(int));
    fifo_queue.front = 0;
    fifo_queue.rear = -1;
    fifo_queue.size = 0;
}

void fifo_push(int frame_index) {
    fifo_queue.rear = (fifo_queue.rear + 1) % MAX_FRAMES;
    fifo_queue.queue[fifo_queue.rear] = frame_index;
    fifo_queue.size++;
}

int fifo_pop() {
    int value = fifo_queue.queue[fifo_queue.front];
    fifo_queue.front = (fifo_queue.front + 1) % MAX_FRAMES;
    fifo_queue.size--;
    return value;
}

// Algoritmos de reemplazo
int find_victim_fifo(Page* frames, int frame_count) {
    return fifo_pop();
}

int find_victim_lru(Page* frames, int frame_count) {
    int least_recent = 0;
    for (int i = 1; i < frame_count; i++) {
        if (frames[i].last_used < frames[least_recent].last_used) {
            least_recent = i;
        }
    }
    return least_recent;
}

int find_victim_clock(Page* frames, int frame_count) {
    while (1) {
        if (!referenced[clock_hand]) {
            int victim = clock_hand;
            clock_hand = (clock_hand + 1) % frame_count;
            return victim;
        }
        referenced[clock_hand] = 0;
        clock_hand = (clock_hand + 1) % frame_count;
    }
}

int find_victim_optimal(Page* frames, int frame_count) {
    int* next_use = (int*)malloc(frame_count * sizeof(int));
    for (int i = 0; i < frame_count; i++) {
        next_use[i] = INT_MAX;
        if (frames[i].page_number != -1) {
            for (int j = 0; j < future_refs_count; j++) {
                if (future_refs[j] == frames[i].page_number) {
                    next_use[i] = j;
                    break;
                }
            }
        }
    }
    
    int victim = 0;
    for (int i = 1; i < frame_count; i++) {
        if (next_use[i] > next_use[victim]) {
            victim = i;
        }
    }
    free(next_use);
    return victim;
}

// Función principal de gestión de memoria
int access_page(Page* frames, int frame_count, PageTable* pt, int page_number, char* algorithm) {
    static int page_faults = 0;
    int frame_index = get_frame(pt, page_number);
    
    if (frame_index == -1) {  // Page fault
        page_faults++;
        
        // Buscar marco libre
        frame_index = -1;
        for (int i = 0; i < frame_count; i++) {
            if (frames[i].page_number == -1) {
                frame_index = i;
                break;
            }
        }
        
        // Si no hay marcos libres, usar algoritmo de reemplazo
        if (frame_index == -1) {
            if (strcmp(algorithm, "FIFO") == 0) {
                frame_index = find_victim_fifo(frames, frame_count);
            } else if (strcmp(algorithm, "LRU") == 0) {
                frame_index = find_victim_lru(frames, frame_count);
            } else if (strcmp(algorithm, "CLOCK") == 0) {
                frame_index = find_victim_clock(frames, frame_count);
            } else if (strcmp(algorithm, "OPT") == 0) {
                frame_index = find_victim_optimal(frames, frame_count);
            }
            
            // Actualizar tabla de páginas para la página víctima
            if (frames[frame_index].page_number != -1) {
                update_page_table(pt, frames[frame_index].page_number, -1);
            }
        }
        
        // Cargar nueva página
        frames[frame_index].page_number = page_number;
        update_page_table(pt, page_number, frame_index);
        
        // Actualizar estado del algoritmo
        if (strcmp(algorithm, "FIFO") == 0) {
            fifo_push(frame_index);
        } else if (strcmp(algorithm, "LRU") == 0) {
            frames[frame_index].last_used = ++clock_ticks;
        } else if (strcmp(algorithm, "CLOCK") == 0) {
            referenced[frame_index] = 1;
        }
    } else {
        // Actualizar estado del algoritmo para página ya en memoria
        if (strcmp(algorithm, "LRU") == 0) {
            frames[frame_index].last_used = ++clock_ticks;
        } else if (strcmp(algorithm, "CLOCK") == 0) {
            referenced[frame_index] = 1;
        }
    }
    
    return page_faults;
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        printf("Uso: ./mvirtual -m <marcos> -a <algoritmo> -f <archivo>\n");
        printf("Algoritmos disponibles: FIFO, LRU, CLOCK, OPT\n");
        return 1;
    }

    int frame_count = 0;
    char* algorithm = NULL;
    char* filename = NULL;

    // Parsear argumentos
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-m") == 0) {
            frame_count = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-a") == 0) {
            algorithm = argv[i + 1];
        } else if (strcmp(argv[i], "-f") == 0) {
            filename = argv[i + 1];
        }
    }

    // Inicializar estructuras
    Page* frames = (Page*)malloc(frame_count * sizeof(Page));
    PageTable page_table;
    init_page_table(&page_table);
    
    for (int i = 0; i < frame_count; i++) {
        frames[i].page_number = -1;
        frames[i].referenced = 0;
        frames[i].last_used = 0;
    }

    // Inicializar algoritmos
    if (strcmp(algorithm, "FIFO") == 0) {
        init_fifo(frame_count);
    } else if (strcmp(algorithm, "CLOCK") == 0) {
        referenced = (int*)calloc(frame_count, sizeof(int));
    }

    // Leer referencias
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: No se pudo abrir el archivo %s\n", filename);
        return 1;
    }

    future_refs = (int*)malloc(MAX_REFS * sizeof(int));
    int page_number;
    while (fscanf(file, "%d", &page_number) != EOF && future_refs_count < MAX_REFS) {
        future_refs[future_refs_count++] = page_number;
    }
    fclose(file);

    // Procesar referencias
    for (int i = 0; i < future_refs_count; i++) {
        int faults = access_page(frames, frame_count, &page_table, future_refs[i], algorithm);
        if (i == future_refs_count - 1) {
            printf("Total de fallos de pagina: %d\n", faults);
        }
    }

    // Liberar memoria
    free(frames);
    free(future_refs);
    if (strcmp(algorithm, "FIFO") == 0) {
        free(fifo_queue.queue);
    } else if (strcmp(algorithm, "CLOCK") == 0) {
        free(referenced);
    }

    return 0;
}