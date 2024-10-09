#include "productor.h"
#include <thread>
#include <chrono>
#include <random>
#include <iostream> 


extern std::ofstream registroProductor; 

Productor::Productor(int id, int npp) : id(id), npp(npp) {}


    void Productor::producir() {
            for (int i = 0; i < npp; i++) {
            // Aquí suponemos que ya tienes los semáforos definidos y correctamente inicializados
            sem_wait(&empty); // Esperar un espacio disponible
            sem_wait(&mutex); // Adquirir acceso al buffer

            int item = id * 100 + i;
            buffer.push_back(std::to_string(item)); // Asegúrate de convertir el item a string
            registroProductor << "Productor " << id << " produjo: " << item << std::endl; // Registro en el archivo
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));

            sem_post(&mutex); // Liberar acceso al buffer
            sem_post(&full); // Incrementar el conteo de ítems
            }
    }
