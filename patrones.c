#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAXLINEAFICHERO 125
#define ALARMAINCREMENOAPUESTASPORCENTAJE 20
#define MAXPATRONES 1000
#define LENGHTMENSAJE 200
#define NUMPATRONES 1

typedef enum boolean {FALSE, TRUE} tBool;

struct patrones {
	void* (* func)(void *);
	struct resultPatrones * resultado;
	char Descripcion[100];
	pthread_t thPatron;
};

char ruta[25];

struct resultPatrones {
  int numeroMensajes;
  char Mensajes[MAXPATRONES][LENGHTMENSAJE];
};

typedef struct nodo{
    char CasaApuestas[100];
    char FechaInicio[20];
    char DiaInicio[20];
    char FechaFin[20];
    char IdUsuario[25];
    char IdJuego[25];
    int TotalParticipacion;
    int TotalApuestas;
} apuesta;
typedef apuesta * pApuestas;

pApuestas LeerFicheroConsolidado (char *NomFichero, int *NumApuestas);
void OrdenarPorJugadorYFechaInicio(pApuestas apuestas, int NumApuestas);
void VisualizarApuestas(pApuestas apuestas, int numApuestas);
void PatronIncrementoPorDia(pApuestas apuestas, int numApuestas, struct resultPatrones *args);

void *Patron_Incremento_Apuestas_Dia_Worker();

int main()
{
	char str[50], *tok, tConf[25];
	FILE *pfich = fopen("fp.conf.txt", "r");                        //Fichero de configuración
	if(pfich == NULL){
		printf( "Error al abrir el fichero de configuración\n");
	}
	else{
	//Leer del fichero de configuración
        	while(fgets(str,50,pfich)!=NULL){
        		tok = strtok(str, "=");
                	strcpy(tConf,tok);
                	tok = strtok(NULL,"=");
                	if(strcmp(tConf,"INVENTORY_FILE")==0){
	        	        strcpy(ruta, tok);
                        	ruta[strlen(ruta)-1]='\0';
                	}
		}
		fclose(pfich);
	}
	struct patrones Patrones[NUMPATRONES] = {
		{&Patron_Incremento_Apuestas_Dia_Worker, calloc (sizeof (struct resultPatrones), 1), "Patron 2"}
	};

	int iPatron;
	int idTuberia, mensajeLenght;	
	
	//Lanzando hilo patrones
	for(iPatron=0; iPatron < NUMPATRONES; iPatron++)
	{
		printf("lanzando hilo patron %s\n", Patrones[iPatron].Descripcion);
		pthread_create(&Patrones[iPatron].thPatron, NULL, Patrones[iPatron].func, Patrones[iPatron].resultado);
	}
	
	//Esperando a que terminen los hilos de los patrones
	for(iPatron=0; iPatron < NUMPATRONES; iPatron++)
	{
		pthread_join(Patrones[iPatron].thPatron, NULL);
	}
	
	//Escribiendo en la tuberia los resultados
    for(iPatron=0; iPatron < NUMPATRONES; iPatron++)
	{
		if (Patrones[iPatron].resultado->numeroMensajes > 0)
		{
			int iMensaje;
			idTuberia = open ("/tmp/patronludopata", O_WRONLY);
			for(iMensaje=0; iMensaje < Patrones[iPatron].resultado->numeroMensajes; iMensaje++)
			{
				mensajeLenght = strlen (Patrones[iPatron].resultado->Mensajes[iMensaje]) + 1;
				write (idTuberia, Patrones[iPatron].resultado->Mensajes[iMensaje], mensajeLenght);
				sleep(3);
				printf("Lanzando a tuberia patron detectado: %s \n", Patrones[iPatron].resultado->Mensajes[iMensaje]);
			}			
			close (idTuberia);	
		}
	}


	printf("fin proceso\n");
}

void *Patron_Incremento_Apuestas_Dia_Worker(void *_args) {
    struct resultPatrones *args = (struct resultPatrones *) _args;
	pApuestas ApuestasConsolidadas;
    int NumeroApuestasConsolidadas = 0;
    ApuestasConsolidadas=LeerFicheroConsolidado(ruta, &NumeroApuestasConsolidadas);
    if (ApuestasConsolidadas== NULL) {
        printf("Se ha producido un error al cargar las apuestas consolidadas\n");
        return NULL;
    }
    OrdenarPorJugadorYFechaInicio(ApuestasConsolidadas, NumeroApuestasConsolidadas);
    // VisualizarApuestas(ApuestasConsolidadas, NumeroApuestasConsolidadas);
    PatronIncrementoPorDia(ApuestasConsolidadas, NumeroApuestasConsolidadas, args);
	pthread_exit(NULL);
    return NULL;
}

pApuestas LeerFicheroConsolidado(char *NomFichero, int *NumApuestas)
{
    char lineaFichero[MAXLINEAFICHERO];

    FILE *pfich;
    pfich = fopen(NomFichero,"rt");
    if (pfich == NULL)
        return NULL;
    else {
        // al ser array necesitamos saber el número de líneas
        int lineas=0;
        fgets(lineaFichero, MAXLINEAFICHERO, pfich); // saltar cabecera
        while(fgets(lineaFichero, MAXLINEAFICHERO, pfich)!=NULL){
            lineas = lineas + 1;
        }
        // Reservamos memoria para el array
        pApuestas pApuesta = (pApuestas)malloc(lineas * sizeof(apuesta));
        // Metemos el número de líneas en el puntero NumEmpleados
        *NumApuestas=lineas;
        // Volver a poner el puntero de lineas del fichero a cero
        fseek(pfich,0,SEEK_SET);
        // Cargamos el array
        char *tock;
        int NumeroApuesta = 0;
        fgets(lineaFichero, MAXLINEAFICHERO, pfich); // saltar cabecera
        while(fgets(lineaFichero, MAXLINEAFICHERO, pfich)!=NULL){

            tock = strtok(lineaFichero,";"); //casaapuesta
            strcpy(pApuesta[NumeroApuesta].CasaApuestas,tock);

            tock = strtok(NULL,";"); //fecha inicio
            strcpy(pApuesta[NumeroApuesta].FechaInicio,tock);

            char DiaInicio[20];
            strncpy(DiaInicio,pApuesta[NumeroApuesta].FechaInicio,10); //copiamos solo el día
            DiaInicio[10] = '\0';

            strncpy(pApuesta[NumeroApuesta].DiaInicio,pApuesta[NumeroApuesta].FechaInicio,10); //copiamos solo el día
            pApuesta[NumeroApuesta].DiaInicio[10] = '\0';

            tock = strtok(NULL,";"); //fecha fin
            strcpy(pApuesta[NumeroApuesta].FechaFin,tock);

            tock = strtok(NULL,";"); //id usuario
            strcpy(pApuesta[NumeroApuesta].IdUsuario,tock);

            tock = strtok(NULL,";"); //id juego
            strcpy(pApuesta[NumeroApuesta].IdJuego,tock);

            tock = strtok(NULL,";"); //TotalParticipacion
            pApuesta[NumeroApuesta].TotalParticipacion = atoi(tock);

            tock = strtok(NULL,";"); //TotalApuestas
            pApuesta[NumeroApuesta].TotalApuestas = atoi(tock);
            NumeroApuesta++;
        }
        return pApuesta;
    }
}

void OrdenarPorJugadorYFechaInicio(pApuestas apuestas, int NumApuestas) {
    int i,j;
    for (i=1;i<NumApuestas;i++)
    {
       for (j=0; j < NumApuestas-i ;j++)
       {
          tBool swap = FALSE;
          if (strcmp(apuestas[j].IdUsuario, apuestas[j+1].IdUsuario)>0) {
            swap =TRUE;
          }
          else if (strcmp(apuestas[j].IdUsuario, apuestas[j+1].IdUsuario)==0) {
              if (strcmp(apuestas[j].FechaInicio, apuestas[j+1].FechaInicio)>0) {
                swap =TRUE;
              }
          }

          if (swap == TRUE) {
            pApuestas aux = (pApuestas)malloc(sizeof(apuesta));
            memcpy(aux,apuestas+j,sizeof(apuesta));
            memcpy(apuestas+j,apuestas+(j+1),sizeof(apuesta));
            memcpy(apuestas+(j+1),aux,sizeof(apuesta));
            free(aux);          }
       }
    }
}

void VisualizarApuestas(pApuestas apuestas, int numApuestas) {
    for(int pivot=0; pivot<numApuestas; pivot++)
    {
      printf("Usuario: %s Fecha Inicio: %s Dia Incio: %s Apuestas: %d \n", apuestas[pivot].IdUsuario, apuestas[pivot].FechaInicio, apuestas[pivot].DiaInicio, apuestas[pivot].TotalApuestas);
    }
}

void PatronIncrementoPorDia(pApuestas apuestas, int numApuestas, struct resultPatrones *args) {
    if (numApuestas > 0)
    {
        char IdUsuarioTmp[25];
        char DiaInicioTmp[20];

        int iPivot = 0;

        while(iPivot<=numApuestas)
        {
            strcpy(IdUsuarioTmp,apuestas[iPivot].IdUsuario);

            int TotalApuestasDiaAnterior = 0;

            while(strcmp(IdUsuarioTmp, apuestas[iPivot].IdUsuario)==0 && iPivot<=numApuestas)
            {
                int TotalApuestasDia = 0;
                strcpy(DiaInicioTmp,apuestas[iPivot].DiaInicio);
                while(strcmp(IdUsuarioTmp, apuestas[iPivot].IdUsuario)==0 && strcmp(DiaInicioTmp, apuestas[iPivot].DiaInicio)==0 && iPivot<=numApuestas)
                {
                    TotalApuestasDia+=apuestas[iPivot].TotalApuestas;
                    iPivot++;
                }

                if (TotalApuestasDiaAnterior>0 && TotalApuestasDia > TotalApuestasDiaAnterior)  //No es la primera
                {
                    float IncrementoEntreDias = ((TotalApuestasDia - (float)TotalApuestasDiaAnterior) / TotalApuestasDiaAnterior) * 100;
                    if (IncrementoEntreDias>=ALARMAINCREMENOAPUESTASPORCENTAJE)
                    {
                        char buf[10];
                        gcvt(IncrementoEntreDias, 6, buf);

                        char Alarma[200];
                        strcpy(Alarma, "El usuario ");
                        strcat(Alarma, IdUsuarioTmp);
                        strcat(Alarma, " Ha superado en ");
                        strcat(Alarma, buf);
                        strcat(Alarma, "% las apuestas en el día ");
                        strcat(Alarma, DiaInicioTmp);
						
						strcpy(args->Mensajes[args->numeroMensajes], Alarma);
						printf("Mensaje %s %d \n",args->Mensajes[args->numeroMensajes], args->numeroMensajes);
						args->numeroMensajes++;

                    }
                }
                TotalApuestasDiaAnterior = TotalApuestasDia;
            }
        }
    }
}
