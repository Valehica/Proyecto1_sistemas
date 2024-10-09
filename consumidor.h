#ifndef CONSUMIDOR_H
#define CONSUMIDOR_H

#include <string>
#include <fstream>
#include <semaphore.h>
#include <vector>

extern sem_t empty, full, mutex;
extern std::vector<std::string> buffer;
extern std::ofstream registroConsumidor;
extern int bufferSize;
extern int out;

class Consumidor {
public:
    Consumidor(int id, int ncc);
    void consumir();
private:
    int id;
    int ncc; // Número de consumos por consumidor
};

#endif
