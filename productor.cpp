#include "productor.h"
#include <thread>
#include <chrono>
#include <random>
#include <iostream>

// Variables externas que manejan el buffer y los semáforos
extern std::vector<std::string> buffer; 
extern sem_t empty, full, mutex;        
extern int in;                          
extern int bufferSize;                  
extern std::ofstream registroProductor;

// Constructor de la clase Productor
Productor::Productor(int id, int npp) : id(id), npp(npp) {}

void Productor::producir() {
    for (int i = 0; i < npp; i++) {
        sem_wait(&empty);
        sem_wait(&mutex);

        int item = id * 100 + i;

        buffer[in] = std::to_string(item);  
        in = (in + 1) % bufferSize;         

        registroProductor << "Productor " << id << " produjo: " << item << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        sem_post(&mutex);
        sem_post(&full);
    }
}
