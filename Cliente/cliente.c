/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeniería de Telecomunicación
Universidad de Jaén

Fichero: cliente.c
Versión: 1.0
Fecha: 23/09/2012
Descripción:
	Cliente de eco sencillo TCP.

Autor: Juan Carlos Cuevas Martínez

*******************************************************/

/* **Pareja de trabajo: Macarena Barseló Vázquez y Begoña Calvo Castillo** */

/* Librerías a utilizar para el correcto funcionamiento del código */
#include <stdio.h>
#include <winsock.h>
#include <time.h>
#include <conio.h>

/* Librería creada para usar en esta práctica */
#include "protocol.h"





int main(int *argc, char *argv[])
{
	SOCKET sockfd; // Definimos un socket, lo llamamos sockfd
	struct sockaddr_in server_in; // Estructura básica para llamadas al sistema y funciones relacionada con direcciones de Internet
	char buffer_in[1024], buffer_out[1024],input[1024]; // Buffers y entrada
	int recibidos=0,enviados=0; // Variables para los datos, ambas iniciadas a 0
	int estado=S_HELO; // Estados de la conexión iniciado a S_HELO
	char option; // Variable condicional para el bucle "do"

	WORD wVersionRequested; // Para trabajar con sockets en Windows
	WSADATA wsaData; // Para trabajar con sockets en Windows
	int err; // Usamos "err" para la comprobación de errores
	char ipdest[16]; // Declaramos la dirección IP destino
	char default_ip[16]="127.0.0.1"; // Dirección IP establecida por defecto
	
	// Inicialización Windows sockets
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup(); // Liberar recursos de la libería
		return(0);
	}
	// Fin: Inicialización Windows sockets
	

	/* Con "do" se crea el socket en el que se recibirán y se enviarán los datos */
	do{ 
		sockfd=socket(AF_INET,SOCK_STREAM,0); 


		if(sockfd==INVALID_SOCKET){ // Si el socket es inválido, mostramos un error por pantalla.
			printf("CLIENTE> ERROR AL CREAR SOCKET\r\n");
			exit(-1); // Salimos del cliente
		}
		else{ // Si el socket es válido, mostramos mensaje por pantalla
			printf("CLIENTE> SOCKET CREADO CORRECTAMENTE\r\n");
			// Pedimos la IP de destino, si pulsamos "Enter" introduciremos la IP por defecto indicada en la parte superior del código		
			printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			gets(ipdest);
			// Para introducir la IP por defecto al pulsar "Enter"
			if(strcmp(ipdest,"")==0)
				strcpy(ipdest,default_ip);

			// Asignamos los valores a la estructura creada al inicio del código, "server_in"
			server_in.sin_family=AF_INET; // Familia de protocolos de Internet
			server_in.sin_port=htons(TCP_SERVICE_PORT); // Puerto del servidor
			server_in.sin_addr.s_addr=inet_addr(ipdest); // Dirección IP del servidor 
			
		    estado=S_HELO; // Se reestablece el estado inicial
		
			// Establece la conexión de transporte
			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0)
			{
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT); // Muestra el mensaje conexión del cliente con la IP destino y el puerto TCP escogidos
			
		
				//Inicio de la máquina de estados
				do{
					switch(estado)
					{
					case S_HELO: // "estado=S_HELO"
						// Se recibe el mensaje de bienvenida
						break;

					case S_USER: // "estado=S_USER"
						// Establece la conexión de aplicación 
						printf("CLIENTE> Introduzca el usuario (enter para salir): "); // Pedimos el usuario por pantalla
						gets(input); 
						if(strlen(input)==0) // Si el usuario es igual a 0, salimos con "estado=S_QUIT"
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);
							estado=S_QUIT;
						}
						else
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",SC,input,CRLF); // Si es válido, podemos continuar
						break;

					case S_PASS: // "estado=S_PASS"
						printf("CLIENTE> Introduzca la clave (enter para salir): ");  // Pedimos la clave por pantalla
						gets(input);
						if(strlen(input)==0) // Si la clave es igual a 0, salimos con "estado=S_QUIT"
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);
							estado=S_QUIT;
						}
						else
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",PW,input,CRLF); // Si es válido, podemos continuar
						break;


					case S_DATA: // "estado=S_DATA"
						printf("CLIENTE> Introduzca datos (enter o QUIT para salir): "); // Pedimos los datos por pantalla
						gets(input);
						if(strlen(input)==0)
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF); // Si los dastos son 0, salimos con "estado=S_QUIT"
							estado=S_QUIT;
						}
						else
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",input,CRLF); // Si es válido, podemos continuar
						break;
				 
				
					}
					// Envío
					if(estado!=S_HELO) 
					// Ejercicio: Comprobar el estado de envio
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);
						// sockfd: socket que utilizamos; buffer_out: buffer que usamos; (int)strlen(buffer_out): tamaño del buffer con los datos que vamos a enviar (en bytes); 0: flag, funcionamiento normal del protocolo.
					// Recepción
					recibidos=recv(sockfd,buffer_in,512,0);
					// socket que utilizamos; buffer_out: buffer que usamos; 512: tamaño del buffer con los datos que vamos a recibir (en bytes); 0: flag, funcionamiento normal del protocolo.
			
					if(recibidos<=0) // Si recibimos 0 ó -1
					{
						DWORD error=GetLastError();
						if(recibidos<0) // Si recibimos un -1, la operación ha fallado
						{
							printf("CLIENTE> Error %d en la recepción de datos\r\n",error);
							estado=S_QUIT; // Salimos con "estado=S_QUIT"
						}
						else // Si recibimos un 0, la conexión ha sido liberada de forma acordada
						{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;
						
					
						}
					}else // Si recibimos la cantidad de bytes enviados
					{
						buffer_in[recibidos]=0x00; // Iniciamos a 0 porque en C los arrays finalizan con el byte 0000 0000
						printf(buffer_in); // Imprimimos por pantalla el valor
						if(estado!=S_DATA && strncmp(buffer_in,OK,2)==0)
							// Si no estamos en "estado=S_DATA" (DATA es el último estado) y hemos recibido un OK, podemos pasar al siguiente estado
							estado++;  
					}
					       
				}while(estado!=S_QUIT); // Se realiza el bucle mientras el estado sea distinto a "S_QUIT"
				
	
		
			}
			else
			{
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT); // Muestra el mensaje de error al conectar el cliente con la IP destino y el puerto TCP escogidos
			}	

			// Fin de la conexión de transporte
			closesocket(sockfd);
			
		}	

		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n"); // Opción para volver a realizar una conexión
		option=_getche();

	}while(option!='n' && option!='N'); 
	// Fin del primer "do", sale de él cuando se introduce por teclado una "n" o una "N"	
	
	return(0);

}
