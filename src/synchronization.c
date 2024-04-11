//Grupo 004
//Francisco Amaro - 54440
//Henrique Rosado - 54433
//Ricardo Pedro - 55309

#include "synchronization.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h> //mmap
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <limits.h>
#include <stdint.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>   
#include <semaphore.h>

/* Função que cria um novo semáforo com nome name e valor inicial igual a
* value. Pode concatenar o resultado da função getuid() a name, para tornar
* o nome único para o processo.
*/
sem_t * semaphore_create(char* name, int value){
    char *nomecompleto = create_dynamic_memory(sizeof(char)+ strlen(name) * sizeof(char) + sizeof(int) );
    sprintf(nomecompleto, "/%s%d", name, getuid());

    sem_t *sem_aux;
    sem_aux = sem_open(nomecompleto, O_CREAT, 0xFFFFFFFF, value);
    
    if(sem_aux==SEM_FAILED){
        perror(nomecompleto);
        exit(6);
        }
        destroy_dynamic_memory(nomecompleto);

    return sem_aux;

}
/* Função que destroi o semáforo passado em argumento.
*/
void semaphore_destroy(char* name, sem_t* semaphore){
    char *nomecompleto = create_dynamic_memory(sizeof(char)+ strlen(name) * sizeof(char) + sizeof(int) );
    sprintf(nomecompleto, "/%s%d", name, getuid());
    
    if(sem_close(semaphore)==-1){
        //perror("error closing semaphore");
        }
    
    if(sem_unlink(nomecompleto)==-1){
        //perror("error unlink semaphore");
        }
        destroy_dynamic_memory(nomecompleto);
}

/* Função que inicia o processo de produzir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_begin(struct prodcons* pc){

    sem_wait(pc -> empty); //vê se há espaço na fila
    sem_wait(pc -> mutex); //concede premissão

}


/* Função que termina o processo de produzir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_end(struct prodcons* pc){
    sem_post(pc -> mutex);
    sem_post(pc -> full); //ver se há coisas para ler na file
}

/* Função que inicia o processo de consumir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_begin(struct prodcons* pc){

    sem_wait(pc -> full);

    sem_wait(pc -> mutex);  

}

/* Função que termina o processo de consumir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_end(struct prodcons* pc){
    sem_post(pc -> mutex);
    sem_post(pc -> empty);
    
}

/* Função que faz wait a um semáforo.
*/
void semaphore_mutex_lock(sem_t* sem){
    sem_wait(sem);
}

/* Função que faz post a um semáforo.
*/
void semaphore_mutex_unlock(sem_t* sem){
    sem_post(sem);
}

