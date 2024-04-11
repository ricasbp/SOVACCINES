//Grupo 004
//Francisco Amaro - 54440
//Henrique Rosado - 54433
//Ricardo Pedro - 55309

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <memory.h>
#include <string.h>

/* Função que reserva uma zona de memória partilhada com tamanho indicado
* por size e nome name, preenche essa zona de memória com o valor 0, e 
* retorna um apontador para a mesma. Pode concatenar o resultado da função
* getuid() a name, para tornar o nome único para o processo.
*/
void* create_shared_memory(char* name, int size){
    int *ptr;
    int ret;
            
    char *nomecompleto = create_dynamic_memory(sizeof(char)+ strlen(name) * sizeof(char) + sizeof(int) );
    sprintf(nomecompleto, "/%s%d", name, getuid());


    int fd = shm_open(nomecompleto, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); if (fd == -1){
        perror(name);
        exit(1);
        //printf("STR_SHM_TERMINATE erro1\n");
    }
    ret = ftruncate(fd, size); if (ret == -1){
        perror(name);
        exit(2);
        //printf("STR_SHM_TERMINATE erro2\n");

    }
    ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); if (ptr == MAP_FAILED) {
        perror("shm-mmap");
        exit(3);
        //printf("STR_SHM_TERMINATE erro4\n");
    }
    if(strcmp(STR_SHM_TERMINATE, name)) {
            //printf("STR_SHM_TERMINATE ola\n");

    }
    destroy_dynamic_memory(nomecompleto);
    return ptr;
}


/* Função que reserva uma zona de memória dinâmica com tamanho indicado
* por size, preenche essa zona de memória com o valor 0, e retorna um 
* apontador para a mesma.
*/
void* create_dynamic_memory(int size){
    void* pointer = calloc(1, size);
    return pointer;
}


/* Função que liberta uma zona de memória dinâmica previamente reservada.
*/
void destroy_shared_memory(char* name, void* ptr, int size){

    char *nomecompleto = create_dynamic_memory(sizeof(char)+ strlen(name) * sizeof(char) + sizeof(int) );
    sprintf(nomecompleto, "/%s%d", name, getuid());
    
    int ret;

    //fazer coisas com a memória partilhada
    ret = munmap(ptr, size); 
    if (ret == -1){
        perror(nomecompleto);
        exit(7);
    }
    ret = shm_unlink(nomecompleto);
    if (ret == -1){
        perror(nomecompleto);
        exit(8);
    }
    
    exit(0);
}

/* Função que liberta uma zona de memória partilhada previamente reservada.
*/
void destroy_dynamic_memory(void* ptr){
    free(ptr);
}

/* Função que escreve uma operação num buffer de acesso aleatório. A
* operação deve ser escrita numa posição livre do buffer, segundo as
* regras de escrita em buffers de acesso aleatório. Se não houver nenhuma
* posição livre, não escreve nada.
*/
void write_rnd_access_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op){
    int n;
    /* produzir um item em next_produced */
    for (n = 0; n < buffer_size; n++) {
        if (buffer -> ptr[n] == 0) {
           buffer -> buff[n] = *op;
           buffer -> ptr[n] = 1;
            break;
        }
     }
}

/* Função que escreve uma operação num buffer circular. A operação deve 
* ser escrita numa posição livre do buffer, segundo as regras de escrita
* em buffers circulares. Se não houver nenhuma posição livre, não escreve
* nada.
*/
void write_circular_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    if((((buffer -> ptr -> in )+ 1) % buffer_size) == buffer -> ptr -> out){
        //buffer cheio, nao queremos escrever nada
    }else{
        buffer -> buff[buffer -> ptr -> in] = *op;
        buffer -> ptr -> in = (buffer -> ptr -> in+ 1) % buffer_size;
    }
}

/* Função que lê uma operação de um buffer de acesso aleatório, se houver
* alguma disponível para ler. A leitura deve ser feita segundo as regras
* de leitura em buffers acesso aleatório. Se não houver nenhuma operação
* disponível, afeta op com o valor -1.
*/
void read_rnd_access_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op){
    int n;
    for (n = 0; n < buffer_size; n++) {
        if (buffer -> ptr[n] == 1 ) {
            *op = buffer -> buff[n];
             buffer -> ptr[n] = 0;
            /* consumir o item em next_consumed */
             break;
        }
    }
    //percorremos o ptr e não consumimos nada 
    if(n == buffer_size){
        op -> id = -1;
    }

}


/* Função que lê uma operação de um buffer circular, se houver alguma 
* disponível para ler. A leitura deve ser feita segundo as regras de
* leitura em buffers circular. Se não houver nenhuma operação disponível,
* afeta op->id com o valor -1.
*/
void read_circular_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op){
    if(buffer -> ptr -> in == buffer -> ptr -> out ){
        //buffer vazio
        op -> id = -1;
    }else{
        *op = buffer -> buff[buffer -> ptr -> out ];
        buffer -> ptr -> out = ((buffer -> ptr -> out ) + 1) % buffer_size;
        /* consumir o item em next_consumed */
    }
}

