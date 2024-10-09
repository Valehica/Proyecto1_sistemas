#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <semaphore.h>
#include <chrono>
#include <random>
#include "productor.h"
#include "consumidor.h"

std::ofstream registroProductor; //txt
std::ofstream registroConsumidor; //txt
sem_t empty, full, mutex; 
/*
    Empty: elementros vacios de buffer 
    Full: elementos llenos del buffer
    mutex: controlador del semaforo   
*/

// Crear memoria compartida para el buffer
void* create_shared_memory(size_t size) {
    int shm_fd = shm_open("/buffer", O_CREAT | O_RDWR, 0666);  // Crear un objeto de memoria compartida
    ftruncate(shm_fd, size);  // Asignar el tamaño de la memoria compartida
    return mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);  // Mapear la memoria compartida
}
int bufferSize = 0;

void init_semaphores(int size) {
    sem_init(&empty, 0, size);  
    sem_init(&full, 0, 0);      
    sem_init(&mutex, 0, 1);     
}

void close_semaphores() {
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cerr << "Número incorrecto de argumentos. Se esperan 5 argumentos.\n";
        return 1;
    }

    int NP = std::stoi(argv[1]);  // Número de productores
    int NC = std::stoi(argv[2]);  // Número de consumidores
    bufferSize = std::stoi(argv[3]);  // Tamaño del buffer
    int NPP = std::stoi(argv[4]);  // Número de producciones por productor
    int NCC = std::stoi(argv[5]);  // Número de consumos por consumidor

    size_t shared_memory_size = bufferSize * sizeof(std::string);
    buffer = static_cast<std::string*>(create_shared_memory(shared_memory_size));

    // Inicialización de semáforos
    init_semaphores(bufferSize);
    

    // Abrir los archivos de registro
    registroProductor.open("registro_productor.txt");
    registroConsumidor.open("registro_consumidor.txt");

    // Crear productores y consumidores
    std::vector<std::thread> productores, consumidores;

    for (int i = 1; i <= NP; ++i) {
        productores.emplace_back([i, NPP]() { Productor(i, NPP); });
        registroProductor << "Productor " << i << " creado\n"; // lo crea en el archivo txt
    }

    for (int i = 1; i <= NC; ++i) {
        consumidores.emplace_back([i, NCC]() { Consumidor(i, NCC); });
        registroConsumidor << "Consumidor " << i << " creado\n"; // lo crea en el archivo txt
    }

    // Esperar que todos terminen
    for (auto& productor : productores) productor.join();
    for (auto& consumidor : consumidores) consumidor.join();

    // Cerrar archivos y semáforos
    registroProductor.close();
    registroConsumidor.close();
    close_semaphores();

        // Liberar memoria compartida
    munmap(buffer, shared_memory_size);
    shm_unlink("/buffer");

    return 0;
}
