#include <iostream>
#include <fstream>
#include <vector>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>
#include "productor.h"
#include "consumidor.h"

std::ofstream registroProductor; // Archivo para los registros del productor
std::ofstream registroConsumidor; // Archivo para los registros del consumidor
sem_t empty, full, mutex; 
/*
    Empty: espacios vacíos en el buffer
    Full: elementos llenos en el buffer
    Mutex: semáforo para garantizar la exclusión mutua
*/
std::vector<std::string> buffer;  // Buffer compartido
int in = 0;   // Índice para escribir (puntero de escritura)
int out = 0;  // Índice para leer (puntero de lectura)
int bufferSize = 0;  // Tamaño del buffer

void init_semaphores(int size) {
    sem_init(&empty, 0, size);  // Espacios vacíos iniciales en el buffer
    sem_init(&full, 0, 0);      // No hay elementos llenos inicialmente
    sem_init(&mutex, 0, 1);     // Exclusión mutua habilitada inicialmente
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

    if (bufferSize < 1) {
        std::cout << "El tamaño del buffer debe ser mayor a 0.\n";
        return 1;
    }

    buffer.resize(bufferSize);  // Ajustar el tamaño del buffer

    // Inicialización de semáforos
    init_semaphores(bufferSize);
    
    // Crear procesos para productores y consumidores
    for (int i = 1; i <= NP; ++i) {
        pid_t pid = fork();  // Crear proceso hijo
        if (pid == 0) {  // Proceso hijo
            std::ofstream registroProductorLocal("registro_productor.txt", std::ios_base::app); // Abrir en modo 'append'
            Productor(i, NPP).producir();
            registroProductorLocal << "Productor " << i << " finalizó\n";
            registroProductorLocal.close();
            _exit(0);  // Terminar proceso hijo
        } else if (pid < 0) {
            std::cerr << "Error al crear proceso para Productor " << i << std::endl;
            return 1;
        }
    }

    for (int i = 1; i <= NC; ++i) {
        pid_t pid = fork();  // Crear proceso hijo
        if (pid == 0) {  // Proceso hijo
            std::ofstream registroConsumidorLocal("registro_consumidor.txt", std::ios_base::app); // Abrir en modo 'append'
            Consumidor(i, NCC).consumir();
            registroConsumidorLocal << "Consumidor " << i << " finalizó\n";
            registroConsumidorLocal.close();
            _exit(0);  // Terminar proceso hijo
        } else if (pid < 0) {
            std::cerr << "Error al crear proceso para Consumidor " << i << std::endl;
            return 1;
        }
    }

    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < NP + NC; ++i) {
        int status;
        wait(&status);  // Esperar la terminación de cada proceso hijo
    }

    // Cerrar semáforos
    close_semaphores();

    return 0;
}
