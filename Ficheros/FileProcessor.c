#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dirent.h>

typedef struct conf{
        char rutaFiles[25];
        char invFile[25];
        char logFile[25];
        int nProc;
        int simSlpMax;
        int simSlpMin;
        char rutaBin[25];
	int nCasas;
}confstruct;

typedef struct {
    char* filename; // Nombre del archivo a leer
    int thread_num; // Número del hilo
}thread_args;

void *funcionThread(void *parametro);

int main(){
char str[50], *tok, tConf[25];
confstruct configuracion;
int interruptor, i;
FILE *pfich = fopen("fp.conf.txt", "r");                        //Fichero de configuración
if(pfich == NULL){
	printf( "Error al abrir el fichero de configuración\n");
}
else{
        printf("Fichero de configuración abierto correctamente\n");
        while(fgets(str,50,pfich)!=NULL){
        	tok = strtok(str, "=");
                strcpy(tConf,tok);
                tok = strtok(NULL,"=");
                if(strcmp(tConf,"PATH_FILES")==0){
	                strcpy(configuracion.rutaFiles, tok);
                        configuracion.rutaFiles[strlen(configuracion.rutaFiles)-1]='\0';
                }
		else if(strcmp(tConf,"INVENTORY_FILE")==0){
                        strcpy(configuracion.invFile, tok);
                        configuracion.invFile[strlen(configuracion.invFile)-1]='\0';
                }
                else if(strcmp(tConf,"LOG_FILE")==0){
                        strcpy(configuracion.logFile, tok);
                        configuracion.logFile[strlen(configuracion.logFile)-1]='\0';
                }
                else if(strcmp(tConf,"NUM_PROCESOS")==0)
                        configuracion.nProc = atoi(tok);
                else if(strcmp(tConf,"SIMULATE_SLEEP_MAX")==0)
                        configuracion.simSlpMax = atoi(tok);
                else if(strcmp(tConf,"SIMULATE_SLEEP_MIN")==0)
                        configuracion.simSlpMin = atoi(tok);
                else if(strcmp(tConf,"PATH_BIN")==0){
                        strcpy(configuracion.rutaBin, tok);
                        configuracion.rutaBin[strlen(configuracion.rutaBin)-1]='\0';
                }
        }
        fclose(pfich);
	pthread_t threads[configuracion.nProc];
	thread_args args[configuracion.nProc];
	for(i=1;i<=configuracion.nProc;i++){
		args[i-1].filename = "";
		args[i-1].thread_num = i;
	}

	DIR *d;
	struct dirent *dir;
	do{
		d = opendir(configuracion.rutaFiles);
		if (d) {
			while ((dir = readdir(d)) != NULL)
			{
				tok = strtok(dir->d_name, "_");
				interruptor = 1;
				for(i = 0; i < configuracion.nProc; i++){
					if(strcmp(args[i].filename,tok)==0)
						interruptor = 0;
				}
				if(strcmp(dir->d_name, "..")!=0 && strcmp(dir->d_name, ".")!=0 && interruptor){
					i=0;
					while(strlen(args[i].filename)!=0 && i < configuracion.nProc){
						printf("%d\n",i);
						i++;
					};
					if(i<configuracion.nProc)
						args[i].filename = tok;
				}
			}
			closedir(d);
			for(i=0; i<configuracion.nProc;i++)
				printf("%s\n", args[i].filename);
		}
	}while(1);

}
}
