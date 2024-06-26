//Grupo 004
//Francisco Amaro - 54440
//Henrique Rosado - 54433
//Ricardo Pedro - 55309

#include "client.h"
#include "synchronization.h"
#include <stdio.h>
#include "sotime.h"
#include <signal.h>

/* Função principal de um Cliente. Deve executar um ciclo infinito
* onde cada iteração do ciclo tem dois passos: 
primeiro,
* lê uma operação
* da main e se a mesma lida tiver id diferente de -1 e se data->terminate
* ainda for igual a 0, processa-a e escreve-a para os proxies; 
segundo,    
* lê uma resposta dos servidores e se a mesma tiver id diferente de -1 e se
* data->terminate ainda for igual a 0, processa-a. 
* Operações com id igual a
* -1 são ignoradas (op inválida) e se data->terminate for igual a 1 é porque
* foi dada ordem de terminação do programa, portanto deve-se fazer return do
* o número de operações processadas. Para efetuar estes passos, pode usar os
* outros métodos auxiliares definidos em client.h. 

* -> conteudo
& -> endereço
*/
int execute_client(int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    signal(SIGINT, SIG_IGN);
    int counter = 0;
    struct operation op; 
    while (1){
        client_get_operation(&op, buffers, data, sems);  
        if(op.id != -1 && *(data -> terminate) == 0){
            client_process_operation(&op, client_id, &counter);
            client_send_operation(&op, buffers, data, sems);
        }

        client_receive_answer(&op, buffers, data, sems);

        if(op.id != -1 && *(data-> terminate) == 0){
            client_process_answer(&op, data, sems);
            
        }
        if(*(data-> terminate) == 1 ){ 
            return counter;   
        }
    }
}

/* Função que lê uma operação do buffer de memória partilhada entre a 
* main e clientes, efetuando a necessária sincronização antes e depois
* de ler. Quando o processo acorda da sincronização, e antes de tentar ler
* a operação, deve verificar se data->terminate tem valor 1. Em caso 
* afirmativo, retorna imediatamente da função.
*/
void client_get_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    consume_begin(sems -> main_cli);
    if( *(data->terminate) != 1) {
        read_rnd_access_buffer(buffers -> main_cli, data -> buffers_size, op);
    }
    consume_end(sems -> main_cli);
}



/* Função que processa uma operação, alterando o seu campo cliente para o id
* passado como argumento, alterando o estado da mesma para 'C' (client), e 
* incrementando o contador de operações.
*/
void client_process_operation(struct operation* op, int cient_id, int* counter) {
    op -> client = cient_id;
    op -> status = 'C';
    *counter = *counter + 1;
    registarInstante(& (op -> client_time));
}


/* Função que escreve uma operação no buffer de memória partilhada entre
* clientes e proxies, efetuando a necessária sincronização antes e depois
* de escrever.
*/
void client_send_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    produce_begin(sems -> cli_prx);
    write_circular_buffer(buffers -> cli_prx, data -> buffers_size, op);
    produce_end(sems -> cli_prx);
}

/* Função que lê uma operação do buffer de memória partilhada entre 
* servidores e clientes, efetuando a necessária sincronização antes e
* depois de ler. Quando o processo acorda da sincronização, e antes de
* tentar ler a operação, deve verificar se data->terminate tem valor 1.
* Em caso afirmativo, retorna imediatamente da função.
*/
void client_receive_answer(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    consume_begin(sems -> srv_cli);
    if( *(data->terminate) != 1) {
        read_circular_buffer(buffers -> srv_cli, data -> buffers_size, op);
    }
    consume_end(sems -> srv_cli);

}
 

/* Função que guarda uma operação no array de operações finalizadas da
* estrutura data, usando o id da mesma para indexar o array. O acesso à
* estrutura deve ser sincronizada através do semáforo sems->results_mutex.
* Imprime também uma mensagem para o terminal a avisar que a operação 
* terminou.
*/
void client_process_answer(struct operation* op, struct main_data* data, struct semaphores* sems) {
    registarInstante(&(op -> end_time));
    
    semaphore_mutex_lock(sems -> results_mutex);
        data -> results[op -> id] = *op;
        
    semaphore_mutex_unlock(sems -> results_mutex);
    
    
    printf ("\nPedido #%d concluído! É agora possível consultar!\n", op -> id);
}



