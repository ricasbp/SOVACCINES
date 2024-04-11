//Grupo 004
//Francisco Amaro - 54440
//Henrique Rosado - 54433
//Ricardo Pedro - 55309

#include "memory.h"
#include "main.h"
#include "client.h"
#include "proxy.h"
#include "server.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "process.h"


/*  PEDIR EXPLICAÇAO DO I NO FOR DO LAUNCHPORCESS */
int launch_process(int process_id, int process_code, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    int pid;

    if ((pid = fork()) == -1) {
        exit(1);
    }
    if (pid == 0) {
        /* Processo filho */
        switch(process_code){
            case 0: //cliente

                /*data -> client_pids[process_id];*/
                //perguntar sobre imports e import do execute!
                exit(execute_client(process_id, buffers, data, sems)); 
                break;
            case 1: //proxy
                exit(execute_proxy(process_id, buffers, data, sems)); 
                break;
            case 2: //servidor
                exit(execute_server(process_id, buffers, data, sems)); 
                break;
            default:
            ;
                
        }
    }
    else {
        return(pid);
    }
}

/* Função que espera que um processo termine através da função waitpid. 
* Devolve o retorno do processo, se este tiver terminado normalmente.
*/                              
int wait_process(int process_id){
    int status;
    waitpid(process_id, &status, 0);

    if (WIFEXITED(status)){
        return WEXITSTATUS(status);
    }
}


