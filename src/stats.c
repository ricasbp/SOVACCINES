#include "stats.h"
#include "main.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include "sotime.h"

void write_statistics_file(char* nome_ficheiro, struct main_data* data, int op_counter) {

    FILE *txtfile;
    printf("\nO NOME DO FICHEIRO è: %s\n", nome_ficheiro);
    txtfile = fopen(nome_ficheiro,"w");
    if(txtfile==NULL) {
        perror("Ficheirodedados.");
        exit(1);
    }
    printf("\nOperation Statistics\n");
    for(int i = 0; i < op_counter; i++){
            fprintf(txtfile, "\nOP: %d\n", data->results[i].id);
            fprintf(txtfile, "Status: %c\n", data->results[i].status);
            fprintf(txtfile, "Client_id: %d\n", data->results[i].client);
            fprintf(txtfile, "Proxy_id: %d\n", data->results[i].proxy);
            fprintf(txtfile, "Server_id: %d\n", data->results[i].server);
            fprintf(txtfile, "Created: ");
            
            char* tempo = create_dynamic_memory(sizeof(char)*50);
            //obterTempoString(&(data -> results[i]).start_time, tempo);
            fprintf(txtfile , "tempo");
            destroy_dynamic_memory(tempo);
            

            fprintf(txtfile, "Client_time: ");

            tempo = create_dynamic_memory(sizeof(char)*50);
            //obterTempoString(&(data -> results[i]).client_time, tempo);
            fprintf(txtfile ,"tempo");
            destroy_dynamic_memory(tempo);

            
            fprintf(txtfile, "Proxy_time: ");


            tempo = create_dynamic_memory(sizeof(char)*50);
            //obterTempoString(&(data -> results[i]).proxy_time, tempo);
            fprintf(txtfile ,"tempo");
            destroy_dynamic_memory(tempo);


            fprintf(txtfile, "Server_time: ");

            tempo = create_dynamic_memory(sizeof(char)*50);
            //obterTempoString(&(data -> results[i]).server_time, tempo);
            fprintf(txtfile ,"tempo");
            destroy_dynamic_memory(tempo);

            
            fprintf(txtfile, "Ended: ");


            tempo = create_dynamic_memory(sizeof(char)*50);
            //obterTempoString(&(data -> results[i]).end_time, tempo);
            fprintf(txtfile ,"tempo");
            destroy_dynamic_memory(tempo);

            
            double diferencaSegundos = (data -> results[i]).end_time.tv_sec - (data -> results[i]).start_time.tv_sec;
            double dif3 = (data -> results[i]).end_time.tv_nsec;
            double dif4 = (data -> results[i]).start_time.tv_nsec;
            double diferencaMiliSecs = (dif3- dif4)/1000000000;
            double diferencaTotal = diferencaSegundos + diferencaMiliSecs;
            fprintf(txtfile, "Total Time: %lf\n", diferencaMiliSecs);
            
            
            
    }

}