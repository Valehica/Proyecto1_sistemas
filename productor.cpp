#include "productor.h"
#include <thread>
#include <chrono>
#include <random>
#include <iostream>

// Variables externas que manejan el buffer y los semáforos
extern std::vector<std::string> buffer; // El buffer circular
extern sem_t empty, full, mutex;        // Semáforos para controlar concurrencia
extern int in;                          // Índice de escritura (para FIFO)
extern std::ofstream registroProductor; // Archivo de registro para los productores

// Constructor de la clase Productor
Productor::Productor(int id, int npp) : id(id), npp(npp) {}

void Productor::producir() {
    for (int i = 0; i < npp; i++) {
        // Crear el ítem con el formato requerido
        std::string item = std::to_string(id) + "_" + std::to_string(i + 1);
        
        // Registrar que el ítem fue generado
        registroProductor << "Productor " << id << " Generó " << item << std::endl;
        
        bool insertado = false;
        while (!insertado) {
            // Intentar insertar en el buffer
            if (sem_trywait(&empty) == 0) { // No se bloquea si no hay espacio
                sem_wait(&mutex);  // Exclusión mutua para acceder al buffer

                // Inserción exitosa
                buffer[in] = item;
                in = (in + 1) % buffer.size();

                // Registrar inserción exitosa
                registroProductor << "Productor " << id << " " << item << " Inserción exitosa" << std::endl;

                // Liberar los semáforos
                sem_post(&mutex);
                sem_post(&full);

                // Marcar el ítem como insertado
                insertado = true;

                // Dormir un tiempo aleatorio entre 0 y 5 segundos después de la inserción exitosa
                std::random_device rd;  
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dist(0, 5000); 
                int sleepTime = dist(gen);
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            } else {
                // Buffer lleno, no pudo insertar
                registroProductor << "Productor " << id << " " << item << " Buffer lleno - Error de inserción" << std::endl;

                // Dormir un tiempo aleatorio entre 0 y 4 segundos antes de reintentar
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dist(0, 4000);
                int sleepTime = dist(gen);
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            }
        }
    }
}
