#include "synchronization.h"
#include "memory.h"
#include "main.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>



void imprimirSignal(struct main_data* data, struct semaphores* sems, int op_counter){
    semaphore_mutex_lock(sems->results_mutex);
        for(int i = 0; i < data -> max_ops; i++){
            int op_id = data -> results[i].id;
            char status = data -> results[i].status;
            long start = data -> results[i].start_time.tv_sec;
            int client = data -> results[i].client;
            long client_time = data -> results[i].client_time.tv_sec;
            int proxy = data -> results[i].proxy;
            long proxy_time = data -> results[i].proxy_time.tv_sec;
            int server = data -> results[i].server;
            long server_time = data -> results[i].server_time.tv_sec;
            long end = data -> results[i].end_time.tv_sec;
           
            if(i < op_counter){
                printf("\nop:%d status:%c start: %ld client:%d client_time:%ld proxy:%d\n proxy_time:%ld server:%d server_time:%ld end:%ld", op_id, status, start, client, client_time, proxy, proxy_time, server, server_time, end );
            }else{
                printf("\nop:%d status:0\n", i);
            }
            
            

    }
    semaphore_mutex_unlock(sems->results_mutex);
    }
