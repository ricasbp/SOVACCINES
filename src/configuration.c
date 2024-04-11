#include "configuration.h"
#include <stdio.h>
#include<stdlib.h>
#include <string.h>

void readFile(char* nome_ficheiro, struct main_data* data) {
    char linha[MAXLINHA];
    FILE *txtfile;
    txtfile = fopen(nome_ficheiro,"r");
    if(txtfile==NULL) {
        perror("Ficheirodedados.");
        exit(1);
    }
    int counter = 0;
    while(fgets(linha,MAXLINHA,txtfile)!=NULL){
        if (counter == 0) {
            data -> max_ops = atoi(linha);
        }
        else if(counter == 1) {
            data -> buffers_size = atoi(linha);
        }
        else if(counter == 2) {
            data -> n_clients = atoi(linha);
        }
        else if(counter == 3) {
            data -> n_proxies = atoi(linha);
        }
        else if(counter == 4) {
            data -> n_servers = atoi(linha);
        }
        else if(counter == 5) {
            data -> log_filename = strdup(linha);
        }
        else if(counter == 6) {

            data -> statistics_filename = strdup(linha);

        }
        else if(counter == 7) {
            data -> alarm_time = atoi(linha);

        }
        counter++;
    }
}