#ifndef PRODUCTOR_H
#define PRODUCTOR_H

#include <string>
#include <fstream>
#include <semaphore.h>
#include <vector>

extern sem_t empty, full, mutex;
extern std::vector<std::string> buffer;
extern std::ofstream registroProductor;
extern int bufferSize;
extern int in;

class Productor {
public:
    Productor(int id, int npp);
    void producir();
private:
    int id;
    int npp; // Número de producciones por productor
};

#endif
