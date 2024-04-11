#include "time.h"

#define NATO_TO_MILLI(N) ((N)/ 1000000)

void registarInstante(struct timespec *instante);


void obterTempo(struct timespec *instante);

//void obterTempoString(struct timespec *instante, char* string);
