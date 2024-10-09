#include "consumidor.h"
#include <thread>
#include <chrono>
#include <random>
#include <iostream>

// Variables externas
extern std::vector<std::string> buffer; // Buffer circular
extern sem_t empty, full, mutex;        // Semáforos para controlar concurrencia
extern int out;                         // Índice de lectura (para FIFO)
extern std::ofstream registroConsumidor; // Archivo de registro para los consumidores

// Constructor de la clase Consumidor
Consumidor::Consumidor(int id, int ncc) : id(id), ncc(ncc) {}

void Consumidor::consumir() {
    for (int i = 0; i < ncc; i++) {
        // Registrar intento de consumir
        registroConsumidor << "Consumidor " << id << " intentando eliminar el elemento del buffer\n";
        
        // Esperar a que haya un elemento disponible en el buffer
        sem_wait(&full);  // Se bloquea si no hay elementos en el buffer
        sem_wait(&mutex); // Exclusión mutua para leer del buffer

        // Leer el ítem del buffer
        std::string item = buffer[out];
        out = (out + 1) % buffer.size();

        // Registrar éxito de la eliminación
        registroConsumidor << "Consumidor " << id << " eliminó con éxito el elemento: " << item << std::endl;

        // Liberar semáforos
        sem_post(&mutex);  // Liberar la exclusión mutua
        sem_post(&empty);  // Incrementar el conteo de espacios vacíos en el buffer

        // Generar un período de espera aleatorio entre 0 y 5 segundos
        std::random_device rd; // Semilla para el generador aleatorio
        std::mt19937 gen(rd()); // Motor de números aleatorios
        std::uniform_int_distribution<> dist(0, 5000); // Distribución entre 0 y 5000 ms
        int sleepTime = dist(gen);

        // Dormir por el tiempo aleatorio generado
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
}
