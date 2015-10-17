/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: servidor.c
Versión: 1.0
Fecha: 23/09/2012
Descripción:
	Servidor de eco sencillo TCP.

Autor: Juan Carlos Cuevas Martínez

*******************************************************/

/* **Pareja de trabajo: Macarena Barselo Vazquez y Begoña Calvo Castillo** */
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <Winsock2.h>

#include "protocol.h"




main()
{

	WORD wVersionRequested; // Para trabajar con sockets en Windows
	WSADATA wsaData;// Para trabajar con sockets en Windows
	SOCKET sockfd,nuevosockfd; //definicion de sockets
	struct sockaddr_in  local_addr,remote_addr; //Estructura basica para llamadas al sistema y funciones relacionada con direcciones de Internet 
	char buffer_out[1024],buffer_in[1024], cmd[10], usr[10], pas[10]; //buffers y entrada de datos
	int err,tamanio; //numeros enteros para la definicion de errores y el tamaño
	int fin=0, fin_conexion=0; 
	int recibidos=0,enviados=0; //inicializacion de las variables recibidos y enviados para el manejo de datos
	int estado=0;

	/** INICIALIZACION DE BIBLIOTECA WINSOCK2 **
	 ** OJO!: SOLO WINDOWS                    **/
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0){
		return(-1);
	}
	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1){
		WSACleanup() ;
		return(-2);
	}
	/** FIN INICIALIZACION DE BIBLIOTECA WINSOCK2 **/


	sockfd=socket(AF_INET,SOCK_STREAM,0);//Creación del socket

	if(sockfd==INVALID_SOCKET)	{ //Si el socket es inválido, no se ha creado bien, se sale
		return(-3);
	}
	else {
		local_addr.sin_family		=AF_INET;			// Familia de protocolos de Internet
		local_addr.sin_port			=htons(TCP_SERVICE_PORT);	// Puerto del servidor 
		//local_addr.sin_addr.s_addr	=htonl(INADDR_ANY);	// Direccion IP del servidor Any cualquier disponible
		local_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
													// Cambiar para que conincida con la del host
	}
	
	
	//bind: Asocia una dirección local con un socket
	// devuelve 0:Proceso de vinculación con el puerto correcto, si no -4 (Error en la vinculación)
	// Enlace el socket a la direccion local (IP y puerto)
	if(bind(sockfd,(struct sockaddr*)&local_addr,sizeof(local_addr))<0)
		return(-4);
	
	//listen:Crea cola de espera para almacenar solicitudes de conexión
	//sockfd: socket local enlazado con a un puerto con bind
	//5, Número máximo de solicitudes de conexión en espera mientras el servidor esté ocupado
	//Se prepara el socket para recibir conexiones y se establece el tamaño de cola de espera
	//devuelve 0 si tuvo éxito, si no. -6
	if(listen(sockfd,5)!=0)
		return (-6);
	
	tamanio=sizeof(remote_addr);

	do
	{
		printf ("SERVIDOR> ESPERANDO NUEVA CONEXION DE TRANSPORTE\r\n");
		
		//Accept: Espera una solicitud de conexión
		//sockfd: socket en el que se escucha el servidor
		//(struct sockaddr*)&remote_addr: estructura con la direccion del cliente
		//tamanio: tamaño de la estructura de dirección del cliente

		//Devuelve: Un entero que representa a un nuevo socket que se usará para la esa conexión o -1(INVALID_SOCKET) si ha habido algún fallo

		nuevosockfd=accept(sockfd,(struct sockaddr*)&remote_addr,&tamanio);

		if(nuevosockfd==INVALID_SOCKET) {//Si ha ocurrido algun fallo, devuelve -5
			
			return(-5);
		}

		printf ("SERVIDOR> CLIENTE CONECTADO\r\nSERVIDOR [IP CLIENTE]> %s\r\nSERVIDOR [CLIENTE PUERTO TCP]>%d\r\n",
					inet_ntoa(remote_addr.sin_addr),ntohs(remote_addr.sin_port)); //mostramos Ip del cliente y el puerto

		//Mensaje de Bienvenida
		sprintf_s (buffer_out, sizeof(buffer_out), "%s Bienvenindo al servidor de ECO%s",OK,CRLF);
		
		//Send:Envía mensaje
		//nuevosocked: Socket local usado para enviar o recibir
		//buffer_out: buffer de datos,
		//(int)strlen(buffer_out): tamaño en bytes del mensaje a enviar
		//0: flag que modifica el comportamiento de la llamada, 0: funcionamiento normal del protocolo
		//Send devuelve La cantidad de bytes enviados o recibidos, 0 si la conexión ha sido liberada de forma acordada o SOCKET_ERROR si la operación ha fallado
		
		enviados=send(nuevosockfd,buffer_out,(int)strlen(buffer_out),0);
/*****************/
		//TODO Comprobar error de envío 

		if(enviados== SOCKET_ERROR) // Si el envio ha fallado, se notifica al cliente del error.
		{
					DWORD error=GetLastError();
					
					printf("CLIENTE> Error %d en la recepcion de datos\r\n",error);
		
					estado=S_QUIT; // Salimos con "estado=S_QUIT"
		}
		else // Si recibimos un 0, la conexion ha sido liberada de forma acordada
		{
					printf("CLIENTE> Conexion con el servidor cerrada\r\n");
					estado=S_QUIT;
					fin_conexion=1;
					
		} 
 /****************/
		//Se reestablece el estado inicial
		estado = S_USER;
		fin_conexion = 0;

		printf ("SERVIDOR> Esperando conexion de aplicacion\r\n");
		do
		{
			//Se espera un comando del cliente

			//Recv:Recibir mensaje
			//nuevosocket: socket usado para recibir datos
			//buffer_in: buffer de datos de entrada
			//1024: tamaño maximo en bytes del buffer
			//flags: modifican el comportamiento de la llamada, 0, funcionamiento normal
			//Valores devueltos: La cantidad de bytes enviados o recibidos.
				//0 si la conexión ha sido liberada de forma acordada.
				//-1 (o SOCKET_ERROR) si la operación ha fallado
			recibidos = recv(nuevosockfd,buffer_in,1023,0);
			//TODO Comprobar posible error de recepción

/****************/			
			if(recibidos<=0) // Si recibimos 0 o -1
					{
						DWORD error=GetLastError();
						if(recibidos<0) // Si recibimos un -1, la operacion ha fallado
						{
							printf("CLIENTE> Error %d en la recepcion de datos\r\n",error);
							estado=S_QUIT; // Salimos con "estado=S_QUIT"
						}
						else // Si recibimos un 0, la conexion ha sido liberada de forma acordada
						{
							printf("Fin de la conexión\r\n");
							estado=S_QUIT;
							fin_conexion=1;  //finalizamos conexion
					
						}



/****************/


			buffer_in[recibidos] = 0x00; // Iniciamos a 0 porque en C los arrays finalizan con el byte 0000 0000
			printf ("SERVIDOR [bytes recibidos]> %d\r\nSERVIDOR [datos recibidos]>%s", recibidos, buffer_in); //mostramos los bytes recibidos y los datos del buffer
			
			switch (estado)
			{
				case S_USER:    /*****************************************/
					strncpy_s ( cmd, sizeof(cmd),buffer_in, 4);
					cmd[4]=0x00; // en C los arrays finalizan con el byte 0000 0000

					if ( strcmp(cmd,SC)==0 ) // si recibido es solicitud de conexion de aplicacion USER usuario
					{
						sscanf_s (buffer_in,"USER %s\r\n",usr,sizeof(usr)); 
						
						// envia OK acepta todos los usuarios hasta que tenga la clave
						sprintf_s (buffer_out, sizeof(buffer_out), "%s%s", OK,CRLF);
						continue;
						
						estado = S_PASS;
						printf ("SERVIDOR> Esperando clave\r\n");
					} else
					if ( strcmp(cmd,SD)==0 )
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexión%s", OK,CRLF);
						fin_conexion=1;
					}
					else
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto%s",ER,CRLF);
					}
				break;

				case S_PASS: /******************************************************/

					
					strncpy_s ( cmd, sizeof(cmd), buffer_in, 4);
					cmd[4]=0x00; // en C los arrays finalizan con el byte 0000 0000

					if ( strcmp(cmd,PW)==0 ) // si comando recibido es password
					{
						sscanf_s (buffer_in,"PASS %s\r\n",pas,sizeof(usr));

						if ( (strcmp(usr,USER)==0) && (strcmp(pas,PASSWORD)==0) ) // si password recibido es correcto
						{
							// envia aceptacion de la conexion de aplicacion, nombre de usuario y
							// la direccion IP desde donde se ha conectado
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s IP(%s)%s", OK, usr, inet_ntoa(remote_addr.sin_addr),CRLF);
							estado = S_DATA;
							printf ("SERVIDOR> Esperando comando\r\n");
						}
						else
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s Autenticación errónea%s",ER,CRLF);
							
							
						}
					} else
					if ( strcmp(cmd,SD)==0 )
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexión%s", OK,CRLF);
						fin_conexion=1;
					}
					else
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto%s",ER,CRLF);
					}
				break;

				case S_DATA: /***********************************************************/
					
					buffer_in[recibidos] = 0x00;
					
					strncpy_s(cmd,sizeof(cmd), buffer_in, 4);

					printf ("SERVIDOR [Comando]>%s\r\n",cmd);
					
					if ( strcmp(cmd,SD)==0 )
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexión%s", OK,CRLF);
						fin_conexion=1;
					}
					else if (strcmp(cmd,SD2)==0)
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Finalizando servidor%s", OK,CRLF);
						fin_conexion=1;
						fin=1;
					}
					else
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto%s",ER,CRLF);
					}
					break;

				default:
					break;
					
			} // switch

			enviados=send(nuevosockfd,buffer_out,(int)strlen(buffer_out),0);
			//TODO 


		} while (!fin_conexion);
		printf ("SERVIDOR> CERRANDO CONEXION DE TRANSPORTE\r\n");
		shutdown(nuevosockfd,SD_SEND);
		closesocket(nuevosockfd);

	}while(!fin);

	printf ("SERVIDOR> CERRANDO SERVIDOR\r\n");

	return(0);
} 
