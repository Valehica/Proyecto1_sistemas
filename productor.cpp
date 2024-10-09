#include "productor.h"
#include <thread>
#include <chrono>
#include <random>
#include <iostream>

// Variables externas que manejan el buffer y los semáforos
extern std::vector<std::string> buffer; // El buffer circular
extern sem_t empty, full, mutex;        // Semáforos para controlar concurrencia
extern int in;                          // Índice de escritura (para FIFO)
extern int bufferSize;                  // Tamaño del buffer
extern std::ofstream registroProductor; // Archivo de registro para los productores

// Constructor de la clase Productor
Productor::Productor(int id, int npp) : id(id), npp(npp) {}

void Productor::producir() {
    for (int i = 0; i < npp; i++) {
        sem_wait(&empty);
        sem_wait(&mutex);

        // Producir el ítem (en este caso, un número basado en el ID del productor y el índice)
        int item = id * 100 + i;

        // Escribir en el buffer de manera circular en la posición 'in'
        buffer[in] = std::to_string(item);  
        in = (in + 1) % bufferSize;         

        registroProductor << "Productor " << id << " produjo: " << item << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        sem_post(&mutex);
        sem_post(&full);
    }
}
