#include "time.h"
#include "sotime.h"
#include <stdlib.h>
#include <stdio.h>

void registarInstante(struct timespec *instante){
    if( clock_gettime( CLOCK_REALTIME, instante) == -1 ) {
        perror( "clock gettime" );
        exit( 0 );
    }
}

//APAGAR ESTA FUNCAO
void obterTempo(struct timespec *instante){
    //, char* timemake
    
    time_t milli;
    struct tm* info;

    milli = NATO_TO_MILLI(instante -> tv_nsec);
    info = localtime(&(instante -> tv_sec));
    //sprintf(time, "%d-%d-%d %d:%d:%d.%d", 1900 + info->tm_year, info -> tm_mon, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec, (int)milli );
    printf("%d-%d-%d %d:%d:%d.%d\n", 1900 + info->tm_year, info -> tm_mon, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec, (int)milli );


}

/*
void obterTempoString(struct timespec *instante, char* string){
    //, char* timemake
    
    time_t milli;
    struct tm* info;

    milli = NATO_TO_MILLI(instante -> tv_nsec);
    info = localtime(&(instante -> tv_sec));
    //sprintf(time, "%d-%d-%d %d:%d:%d.%d", 1900 + info->tm_year, info -> tm_mon, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec, (int)milli );
    FILE *txtfile;
    txtfile = fopen(nome_ficheiro,"w");
    if(txtfile==NULL) {
        perror("Ficheirodedados.");
        exit(1);
    }
    fprintf(string, "%d-%d-%d %d:%d:%d.%d\n", 1900 + info->tm_year, info -> tm_mon, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec, (int)milli );


}
**/
