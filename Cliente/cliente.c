/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: cliente.c
Versión: 1.0
Fecha: 23/09/2012
Descripción:
	Cliente de eco sencillo TCP.

Autor: Juan Carlos Cuevas Martínez

*******************************************************/

/* **Pareja de trabajo: Macarena Barselo Vazquez y Begoña Calvo Castillo** */
#include <stdio.h>
#include <winsock.h>
#include <time.h>
#include <conio.h>

#include "protocol.h"





int main(int *argc, char *argv[])
{
	SOCKET sockfd; // Definimos un socket, lo llamamos sockfd
	struct sockaddr_in server_in; // Estructura basica para llamadas al sistema y funciones relacionada con direcciones de Internet
	char buffer_in[1024], buffer_out[1024],input[1024]; // Buffers y entrada de datos
	int recibidos=0,enviados=0; // Variables para el envio y recepcion de los datos, ambas iniciadas a 0
	int estado=S_HELO; // Estados de la conexion iniciado a S_HELO
	char option; // Variable condicional para el bucle "do"
	WORD wVersionRequested; // Para trabajar con sockets en Windows
	WSADATA wsaData; // Para trabajar con sockets en Windows
	int err; // Usamos "err" para la comprobacion de errores
    char ipdest[16];// Declaramos la direccion IP destino
	char default_ip[16]="127.0.0.1"; // Direccion IP establecida por defecto
	
	//Inicialización Windows sockets
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();  // Liberar recursos de la liberia
		return(0);
	}
	//Fin: Inicialización Windows sockets

	/* Con "do" se crea el socket en el que se recibiran y se enviaran los datos */

	do{
		sockfd=socket(AF_INET,SOCK_STREAM,0);

		if(sockfd==INVALID_SOCKET) // Si el socket es invalido, mostramos un error por pantalla.
		{
			printf("CLIENTE> ERROR AL CREAR SOCKET\r\n");
			exit(-1);
		} 
		else // Si el socket es valido, mostramos mensaje por pantalla
		{
			printf("CLIENTE> SOCKET CREADO CORRECTAMENTE\r\n");

			// Pedimos la IP de destino, si pulsamos "Enter" introduciremos la IP por defecto indicada en la parte superior del codigo		
			printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			gets(ipdest);

			// Para introducir la IP por defecto al pulsar "Enter"
			if(strcmp(ipdest,"")==0)
				strcpy(ipdest,default_ip);

			// Asignamos los valores a la estructura creada al inicio del codigo, "server_in"
			server_in.sin_family=AF_INET; // Familia de protocolos de Internet
			server_in.sin_port=htons(TCP_SERVICE_PORT); // Puerto del servidor
			server_in.sin_addr.s_addr=inet_addr(ipdest); // Direccion IP del servidor
			
			estado=S_HELO; // Se reestablece el estado inicial
		
			// establece la conexion de transporte
			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0)
			{
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			
		
				//Inicio de la máquina de estados

				do{
					/*Comprobacion de autenticacion*/
						if(estado==S_PASS &&strncmp(buffer_in,OK,2)!=0)
						{ /*Comprobacion autenticacion*/
								estado=S_USER; //si el estado PASS no nos ha devuelto un OK (usuario y password OK), volvemos al estado USER
						}
		
					switch(estado)
					{

					case S_HELO:
						// Se recibe el mensaje de bienvenida
						break;

					case S_USER:
						// establece la conexion de aplicacion 
						printf("CLIENTE> Introduzca el usuario (enter para salir): ");  // Pedimos el usuario por pantalla
						gets(input); //lee todo lo introducido hasta que se presione ENTER
						if(strlen(input)==0) // Si no se introduce nada, sale con "estado=S_QUIT" (SD)
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);
							estado=S_QUIT;
						}
						else

						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",SC,input,CRLF); // Si se introduce algo, se almacena en el buffer el usuario (SC)=USER
						break;

					case S_PASS:
						printf("CLIENTE> Introduzca la clave (enter para salir): ");  // Pedimos la clave por pantalla
						gets(input);
						if(strlen(input)==0) // Si no se introduce nada, salimos con "estado=S_QUIT"
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF); 
							estado=S_QUIT;
						}
						else
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",PW,input,CRLF); // Se almacena en el buffer la contraseña
						}
						break;

					case S_DATA:
						printf("CLIENTE> Introduzca datos: enter o QUIT para salir, SUMA para realizar una suma: "); // Pedimos los datos por pantalla, QUIT o enter cierra la conexion con el servidor, y SUMA realizamos la suma
						gets(input);
						if(strlen(input)==0) //Si no se introduce nada
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF); // Salimos con "estado=S_QUIT"
							estado=S_QUIT;
						}
						else if(strcmp(input,SD)==0) //Si se introduce QUIT
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF); // Salimos con "estado=S_QUIT"
							estado=S_QUIT;
						}
						else if (strcmp(input,SUM)==0) //Se ha introducido SUMA
						{
							printf("Introduce dos numeros para realizar la suma de ambos, como máximo, de 4 cifras cada uno:\n ");
							gets(input);
														
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",SUM,input,CRLF); //los guardarmos en el buffer de salida
						}
						else
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",input,CRLF);
							break;
				 
				
					}
			
					//Envio
					if(estado!=S_HELO){
					// Ejercicio: Comprobar el estado de envio
						// sockfd: socket que utilizamos
						// buffer_out: buffer que usamos 
						//(int)strlen(buffer_out): tamaño del buffer con los datos que vamos a enviar (en bytes)
						// 0: flag, funcionamiento normal del protocolo.
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);	
						if(enviados==SOCKET_ERROR || enviados==0)
						{
							if(enviados==SOCKET_ERROR)
							{
							DWORD error=GetLastError();
							printf("CLIENTE> Error %d en el envío de datos\r\n",error);
							estado=S_QUIT;
							}
							else
							{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;
							}
						}
					}
						
					//Recibo
					// sockfd: socket que utilizamos
					//buffer_out: buffer que usamos 
					//512: tamaño del buffer con los datos que vamos a recibir (en bytes)
					// 0: flag, funcionamiento normal del protocolo.
					recibidos=recv(sockfd,buffer_in,512,0);

					if(recibidos<=0) // Si recibimos 0 o -1
					{
						DWORD error=GetLastError();
						if(recibidos<0) // Si recibimos un -1 o SOCKET_ERROR la operacion ha fallado
						{
							printf("CLIENTE> Error %d en la recepción de datos\r\n",error);  // Salimos con "estado=S_QUIT"
							estado=S_QUIT; 
						}
						else // Si recibimos un 0, la conexion ha sido liberada de forma acordada
						{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;
						
					
						}
					}else{  // Si recibimos la cantidad de bytes enviados
						buffer_in[recibidos]=0x00; // Iniciamos a 0 porque en C los arrays finalizan con el byte 0000 0000
						printf(buffer_in); // Imprimimos por pantalla el valor
						if(estado!=S_DATA && strncmp(buffer_in,OK,2)==0) // Si no estamos en "estado=S_DATA" (DATA es el último estado) y hemos recibido un OK, podemos pasar al siguiente estado
							estado++;  
					}

				}while(estado!=S_QUIT); // Se realiza el bucle mientras el estado sea distinto a "S_QUIT"
			}
			else
			{
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT); // Muestra el mensaje de error al conectar el cliente con la IP destino y el puerto TCP escogidos
			}		
			// fin de la conexion de transporte
			closesocket(sockfd);
			
		}	
		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n"); // Opcion para volver a realizar una conexion
		option=_getche();

	}while(option!='n' && option!='N');
	// Fin del primer "do", sale de el cuando se introduce por teclado una "n" o una "N"
	
	
	return(0);

}
