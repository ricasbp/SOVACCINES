//Grupo 004
//Francisco Amaro - 54440
//Henrique Rosado - 54433
//Ricardo Pedro - 55309

#include "memory.h"
#include "main.h"
#include "synchronization.h"
#include "process.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "sotime.h"
#include "sosignal.h"
#include <signal.h>
#include "configuration.h"
#include "stats.h"


struct main_data* data;
struct communication_buffers* buffers;
struct semaphores* sems;
int op_counter = 0;

void main_args(int argc, char* argv[], struct main_data* data){

    readFile(argv[1], data);

}

/* Função que reserva a memória dinâmica necessária para a execução
* do sovaccines, nomeadamente para os arrays *_pids e *_stats da estrutura 
* main_data. Para tal, pode ser usada a função create_dynamic_memory do memory.h.
*/
void create_dynamic_memory_buffers(struct main_data* data){
    data -> client_pids = create_dynamic_memory(data -> n_clients * sizeof(int));	//process ids de clientes
	data -> proxy_pids	 = create_dynamic_memory(data -> n_proxies * sizeof(int));//process ids de proxies
	data -> server_pids  = create_dynamic_memory(data -> n_servers * sizeof(int));	//process ids de servidores
	
	data -> client_stats  = create_dynamic_memory(data -> n_clients * sizeof(int));	//nº de operações recebidas por cada cliente
	data -> proxy_stats = create_dynamic_memory(data -> n_proxies * sizeof(int));	//nº de operações encaminhadas por cada proxy
	data -> server_stats  = create_dynamic_memory(data -> n_servers * sizeof(int));	//nº de operações respondidas por cada servidor
	
}

/* Função que reserva a memória partilhada necessária para a execução do
* sovaccines. É necessário reservar memória partilhada para todos os buffers da
* estrutura communication_buffers, incluindo os buffers em si e respetivos
* pointers, assim como para o array data->results e variável data->terminate.
* Para tal, pode ser usada a função create_shared_memory do memory.h.
*/
void create_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers){

    (buffers -> main_cli -> buff) = create_shared_memory(STR_SHM_MAIN_CLI_BUFFER, data -> buffers_size * sizeof(struct operation));
    (buffers -> main_cli -> ptr) = create_shared_memory(STR_SHM_MAIN_CLI_PTR, data -> buffers_size * sizeof(int));

    (buffers -> cli_prx -> buff) = create_shared_memory(STR_SHM_CLI_PRX_BUFFER, data -> buffers_size * sizeof(struct operation));
    (buffers -> cli_prx -> ptr) = create_shared_memory(STR_SHM_CLI_PRX_PTR, sizeof(struct in_out));

    (buffers -> prx_srv -> buff) = create_shared_memory(STR_SHM_PRX_SRV_BUFFER, data -> buffers_size * sizeof(struct operation));
    (buffers -> prx_srv -> ptr) = create_shared_memory(STR_SHM_PRX_SRV_PTR, data -> buffers_size * sizeof(int));


    (buffers -> srv_cli -> buff) = create_shared_memory(STR_SHM_SRV_CLI_BUFFER, data -> buffers_size * sizeof(struct operation));
    (buffers -> srv_cli -> ptr) = create_shared_memory(STR_SHM_SRV_CLI_PTR, sizeof(struct in_out));

    data -> results = create_shared_memory(STR_SHM_RESULTS, data->max_ops * sizeof(struct operation));
    //array ou nao?
    data -> terminate = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));
    *(data -> terminate) = 0;

}

/* Função que liberta todos os buffers de memória partilhada previamente
* reservados nas estruturas data e buffers.
*/
void destroy_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers){
    destroy_shared_memory(STR_SHM_MAIN_CLI_BUFFER, buffers -> main_cli -> buff, data -> buffers_size * sizeof(struct operation));
    destroy_shared_memory(STR_SHM_MAIN_CLI_PTR, buffers -> main_cli -> ptr, data -> buffers_size * sizeof(int));

    destroy_shared_memory(STR_SHM_CLI_PRX_BUFFER, buffers -> cli_prx -> buff, data -> buffers_size * sizeof(struct operation));
    destroy_shared_memory(STR_SHM_CLI_PRX_PTR, buffers -> cli_prx -> ptr, sizeof(struct in_out));

    destroy_shared_memory(STR_SHM_PRX_SRV_BUFFER, buffers -> prx_srv -> buff, data -> buffers_size * sizeof(struct operation));
    destroy_shared_memory(STR_SHM_PRX_SRV_PTR, buffers -> prx_srv -> buff, data -> buffers_size * sizeof(int));

    destroy_shared_memory(STR_SHM_SRV_CLI_BUFFER, buffers -> srv_cli -> buff, data -> buffers_size * sizeof(struct operation));
    destroy_shared_memory(STR_SHM_SRV_CLI_PTR, buffers -> srv_cli -> ptr, sizeof(struct in_out));

    destroy_shared_memory(STR_SHM_RESULTS, data -> results, data->max_ops * sizeof(struct operation) );

    destroy_shared_memory(STR_SHM_TERMINATE, data -> terminate, sizeof(int));

}

/* Função que inicializa os semáforos da estrutura semaphores. Semáforos
* *_full devem ser inicializados com valor 0, semáforos *_empty com valor
* igual ao tamanho dos buffers de memória partilhada, e os *_mutex com valor
* igual a 1. Para tal pode ser usada a função semaphore_create.
*/
void create_semaphores(struct main_data* data, struct semaphores* sems){
 int valor;
 destroy_semaphores(sems);
 sems -> main_cli -> full = semaphore_create(STR_SEM_MAIN_CLI_FULL,0);
 sem_getvalue(sems->main_cli->full, &valor);
 sems -> main_cli -> empty = semaphore_create(STR_SEM_MAIN_CLI_EMPTY,data -> buffers_size);
 sems -> main_cli -> mutex = semaphore_create(STR_SEM_MAIN_CLI_MUTEX,1);
 sems -> cli_prx -> full = semaphore_create(STR_SEM_CLI_PRX_FULL,0);
 sems -> cli_prx -> empty = semaphore_create(STR_SEM_CLI_PRX_EMPTY,data -> buffers_size);
 sems -> cli_prx -> mutex = semaphore_create(STR_SEM_CLI_PRX_MUTEX,1);
 sems -> prx_srv -> full = semaphore_create(STR_SEM_PRX_SRV_FULL,0);
 sems -> prx_srv -> empty = semaphore_create(STR_SEM_PRX_SRV_EMPTY,data -> buffers_size);
 sems -> prx_srv -> mutex = semaphore_create(STR_SEM_PRX_SRV_MUTEX ,1);
 sems -> srv_cli -> full = semaphore_create(STR_SEM_SRV_CLI_FULL,0);
 sems -> srv_cli -> empty = semaphore_create(STR_SEM_SRV_CLI_EMPTY  ,data -> buffers_size);
 sems -> srv_cli -> mutex = semaphore_create(STR_SEM_SRV_CLI_MUTEX,1);
 sems -> results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX,1);



}

/* Função que inicia os processos dos clientes, proxies e
* servidores. Para tal, pode usar a função launch_process,
* guardando os pids resultantes nos arrays respetivos
* da estrutura data.
*/
void launch_processes(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
int i;
for(i = 0; i < data -> n_clients; i++)
    data->client_pids[i] = launch_process(i, 0, buffers, data, sems);

for(i = 0; i < data -> n_proxies; i++)
    data->proxy_pids[i] = launch_process(i, 1, buffers, data, sems);

for(i = 0; i < data -> n_servers; i++)
    data->server_pids[i] = launch_process(i, 2, buffers, data, sems);        
}

/* Função que faz interação do utilizador com o sistema, podendo receber 4 comandos:
* op - cria uma nova operação, através da função create_request
* read - verifica o estado de uma operação através da função read_answer
* stop - termina o execução do sovaccines através da função stop_execution
* help - imprime informação sobre os comandos disponiveis
*/
void user_interaction(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    printf("\n Ações disponíveis:");
    printf("\n       op - criar um pedido de aquisição de vacinas");
    printf("\n       read x - consultar o estado do pedido x");
    printf("\n       stop - termina a execução do sovaccines.");
    printf("\n       help - imprime informação sobre as ações disponíveis.");

    char str[5];
    char op[] = "op";
    char read[] = "read";
    char stop[] = "stop";
    char help[] = "help";
   
    int acabou = 1;
    
    while(acabou) {
        printf("\nIntroduzir ação:\n ");
            scanf("%s",str);
    if(strcmp(str, op) == 0) {
        create_request(&op_counter, buffers, data, sems);
    }
    else if(strcmp(str, read) == 0) {
        read_answer(data, sems);
    }
    else if(strcmp(str, stop) == 0) {

        
        

        stop_execution(data, buffers, sems);
        acabou = 0;
    }
    else if(strcmp(str, help) == 0) {
        printf("\n Ações disponíveis:");
        printf("\n       op - criar um pedido de aquisição de vacinas\n");
        printf("\n       read x - consultar o estado do pedido x\n");
        printf("\n       stop - termina a execução do sovaccines.");
        printf("\n       help - imprime informação sobre as ações disponíveis.\n");
        printf("Introduzir ação:");


    }
    usleep(1000);
    }
    
   
    
}

void print_estatisticas_detailed() {
    write_statistics_file(data->statistics_filename, data, op_counter);

    printf("\nOperation Statistics\n");
    for(int i = 0; i < op_counter; i++){
            printf("\nOP: %d\n", data->results[i].id);
            printf("Status: %c\n", data->results[i].status);
            printf("Client_id: %d\n", data->results[i].client);
            printf("Proxy_id: %d\n", data->results[i].proxy);
            printf("Server_id: %d\n", data->results[i].server);
            printf("Created: ");
            obterTempo(&(data -> results[i]).start_time);
            printf("Client_time: ");
            obterTempo(&(data -> results[i]).client_time);
            printf("Proxy_time: ");
            obterTempo(&(data -> results[i]).proxy_time);
            printf("Server_time: ");
            obterTempo(&(data -> results[i]).server_time);
            printf("Ended: ");
            obterTempo(&(data -> results[i]).end_time);
            double diferencaSegundos = (data -> results[i]).end_time.tv_sec - (data -> results[i]).start_time.tv_sec;
            double dif3 = (data -> results[i]).end_time.tv_nsec;
            double dif4 = (data -> results[i]).start_time.tv_nsec;
            double diferencaMiliSecs = (dif3- dif4)/1000000000;
            double diferencaTotal = diferencaSegundos + diferencaMiliSecs;
            printf("Total Time: %lf\n", diferencaMiliSecs);
            
            
            
    }
}

/* Se o limite de operações ainda não tiver sido atingido, cria uma nova
* operação identificada pelo valor atual de op_counter, escrevendo a mesma
* no buffer de memória partilhada entre main e clientes e efetuando a 
* necessária sincronização antes e depois de escrever. Imprime o id da
* operação e incrementa o contador de operações op_counter.
*/
void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){

if(*op_counter < data -> max_ops){
    
    struct operation aux;
    aux.id = *op_counter;
    
    registarInstante(&aux.start_time);
    printf("O pedido #%d foi criado!\n", aux.id);

    produce_begin(sems -> main_cli);
    write_rnd_access_buffer(buffers -> main_cli, data -> buffers_size, &aux);
    produce_end(sems -> main_cli);

    *op_counter = *op_counter + 1; //alteras os outros do client, server proxy
}
}

/* Função que lê um id de operação do utilizador e verifica se a mesma
* é valida e se já foi respondida por um servidor. Em caso afirmativo,
* imprime informação da mesma, nomeadamente o seu estado, e os ids do 
* cliente, proxy e servidor que a processaram. O acesso à estrutura 
* data->results deve ser sincronizado com as funções e semáforos
* respetivos.
*/
void read_answer(struct main_data* data, struct semaphores* sems){
    int id_dado_pelo_user;
    scanf("%d", &id_dado_pelo_user);
    semaphore_mutex_lock(sems->results_mutex);
    if(    data->results[id_dado_pelo_user].id != -1 &&  data -> results[id_dado_pelo_user].status == 'S' ) {
        if(id_dado_pelo_user < op_counter){
            printf("Op %d com estado %c foi recebida pelo cliente %d, encaminhada pelo proxy %d, e tratada pelo servidor %d!", data -> results[id_dado_pelo_user].id,  data -> results[id_dado_pelo_user].status, data -> results[id_dado_pelo_user].client, data -> results[id_dado_pelo_user].proxy, data -> results[id_dado_pelo_user].server );
        }else{
            printf("\nA operação ainda nao foi executada!\n");
        }
    }
    semaphore_mutex_unlock(sems->results_mutex);
}

/* Função que termina a execução do programa sovaccines. Deve começar por 
* afetar a flag data->terminate com o valor 1. De seguida, e por esta
* ordem, deve acordar processos adormecidos, esperar que terminem a sua 
* execução, escrever as estatisticas finais do programa, e por fim libertar
* os semáforos e zonas de memória partilhada e dinâmica previamente 
*reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
*/
void stop_execution(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems){
    *(data -> terminate)  = 1;
    wakeup_processes(data, sems);
    wait_processes(data);
    write_statistics(data);
    print_estatisticas_detailed();
    destroy_dynamic_memory_buffers(data);
    destroy_shared_memory_buffers(data, buffers);
    destroy_semaphores(sems);
}

/* Função que acorda todos os processos adormecidos em semáforos, para que
* estes percebam que foi dada ordem de terminação do programa. Para tal,
* pode ser usada a função produce_end sobre todos os conjuntos de semáforos
* onde possam estar processos adormecidos e um número de vezes igual ao 
* máximo de processos que possam lá estar.
*/
void wakeup_processes(struct main_data* data, struct semaphores* sems){
    for (int i = 0 ; i < data -> n_clients + data -> n_proxies + data -> n_servers ; i++) {
        produce_end(sems -> main_cli);
        produce_end(sems -> cli_prx);
        produce_end(sems -> prx_srv);
        produce_end(sems -> srv_cli);
        semaphore_mutex_unlock(sems -> results_mutex);
    }
    

}

/* Função que espera que todos os processos previamente iniciados terminem,
* incluindo clientes, proxies e servidores. Para tal, pode usar a função 
* wait_process do process.h.
*/
void wait_processes(struct main_data* data){
    int i;
for(i = 0; i < data -> n_clients; i++)
    data -> client_stats[i] = wait_process(data -> client_pids[i]);

for(i = 0; i < data -> n_proxies; i++)
    data -> proxy_stats[i] = wait_process(data -> proxy_pids[i]);

for(i = 0; i < data -> n_servers; i++)
    data -> server_stats[i] = wait_process(data -> server_pids[i]);
}


/* Função que imprime as estatisticas finais do sovaccines, nomeadamente quantas
* operações foram processadas por cada cliente, proxy e servidor.
*/
void write_statistics(struct main_data* data){
        int i;
        printf("Process Statistics\n");
        for(i = 0; i < data -> n_clients; i++)
            printf("\n Client %d received %d requests!\n", i, data -> client_stats[i]);
        for(i = 0; i < data -> n_proxies; i++)
            printf("\n Proxy %d forwarded %d requests! \n", i, data -> proxy_stats[i]);

        for(i = 0; i < data -> n_servers; i++)
            printf("\n Server %d responded to %d requests! \n", i, data -> server_stats[i]);   


        

}

/* Função que liberta todos os buffers de memória dinâmica previamente
* reservados na estrutura data.
*/
void destroy_dynamic_memory_buffers(struct main_data* data){
    destroy_dynamic_memory(data -> client_pids);
    destroy_dynamic_memory(data -> proxy_pids);
    destroy_dynamic_memory(data -> server_pids);
	
    destroy_dynamic_memory(data -> client_stats);
    destroy_dynamic_memory(data -> proxy_stats);
    destroy_dynamic_memory(data -> server_stats);

}




/* Função que liberta todos os semáforos da estrutura semaphores.
*/
void destroy_semaphores(struct semaphores* sems){
    semaphore_destroy(STR_SEM_MAIN_CLI_FULL, sems -> main_cli -> full);
    semaphore_destroy(STR_SEM_MAIN_CLI_EMPTY, sems -> main_cli -> empty);
    semaphore_destroy(STR_SEM_MAIN_CLI_MUTEX, sems -> main_cli -> mutex);

    semaphore_destroy(STR_SEM_CLI_PRX_FULL, sems -> cli_prx -> full );
    semaphore_destroy(STR_SEM_CLI_PRX_EMPTY, sems -> cli_prx -> empty);
    semaphore_destroy(STR_SEM_CLI_PRX_MUTEX, sems -> cli_prx -> mutex);

    semaphore_destroy(STR_SEM_PRX_SRV_FULL, sems -> prx_srv -> full);
    semaphore_destroy(STR_SEM_PRX_SRV_EMPTY, sems -> prx_srv -> empty);
    semaphore_destroy(STR_SEM_PRX_SRV_MUTEX, sems -> prx_srv -> mutex);

    semaphore_destroy(STR_SEM_SRV_CLI_FULL, sems -> srv_cli -> full);
    semaphore_destroy(STR_SEM_SRV_CLI_EMPTY , sems -> srv_cli -> empty);
    semaphore_destroy(STR_SEM_SRV_CLI_MUTEX, sems -> srv_cli -> mutex);
   
    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems -> results_mutex);
    
}

void ctrlC() {
    signal(SIGINT, ctrlC);
    stop_execution(data, buffers, sems);
}


void sinal_op(){
    imprimirSignal(data, sems, op_counter);
    signal(SIGALRM, sinal_op);
    alarm(10);
}


int main(int argc, char *argv[]) {
    signal(SIGALRM, sinal_op);
    alarm(10);

    char *nameData = "dataGlobal";
    char *nameBuffers = "buffersGlobal";
    char *nameSems = "semsGlobal";

    

    signal(SIGINT, ctrlC);
    //init data structures
    data = create_shared_memory(nameData, sizeof(struct main_data));
    data -> log_filename = create_dynamic_memory(50 * sizeof(char)); //LEMBRAR DE DESTRUIR ESTA STRING
    data -> statistics_filename = create_dynamic_memory(50 * sizeof(char));	
    buffers = create_shared_memory(nameBuffers, sizeof(struct communication_buffers));
    buffers->main_cli = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->cli_prx = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->prx_srv = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->srv_cli = create_dynamic_memory(sizeof(struct circular_buffer));
    sems = create_shared_memory(nameSems, sizeof(struct semaphores));
    sems->main_cli = create_dynamic_memory(sizeof(struct prodcons));
    sems->cli_prx = create_dynamic_memory(sizeof(struct prodcons));
    sems->prx_srv = create_dynamic_memory(sizeof(struct prodcons));
    sems->srv_cli = create_dynamic_memory(sizeof(struct prodcons));

    //execute main code
    main_args(argc, argv, data);
    
    create_dynamic_memory_buffers(data);
    
    create_shared_memory_buffers(data, buffers);

    create_semaphores(data, sems);


    launch_processes(buffers, data, sems);
    user_interaction(buffers, data, sems);
    
    //release final memory
    destroy_shared_memory(nameData, data, sizeof(struct main_data));
    destroy_dynamic_memory(buffers->main_cli);
    destroy_dynamic_memory(buffers->cli_prx);
    destroy_dynamic_memory(buffers->prx_srv);
    destroy_dynamic_memory(buffers->srv_cli);
    destroy_shared_memory(nameBuffers, buffers, sizeof(struct communication_buffers));
    destroy_dynamic_memory(sems->main_cli);
    destroy_dynamic_memory(sems->cli_prx);
    destroy_dynamic_memory(sems->prx_srv);
    destroy_dynamic_memory(sems->srv_cli);
    destroy_shared_memory(nameSems, sems, sizeof(struct semaphores));
    destroy_dynamic_memory(data -> log_filename);
    destroy_dynamic_memory(data -> statistics_filename);
}
