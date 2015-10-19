/*******************************************************
Protocolos de Transporte
Grado en Ingenier�a Telem�tica
Dpto. Ingen�er�a de Telecomunicaci�n
Univerisdad de Ja�n

Fichero: servidor.c
Versi�n: 1.0
Fecha: 23/09/2012
Descripci�n:
	Servidor de eco sencillo TCP.

Autor: Juan Carlos Cuevas Mart�nez

*******************************************************/

/* **Pareja de trabajo: Macarena Barselo Vazquez y Bego�a Calvo Castillo** */
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <Winsock2.h>
#include "protocol.h"




main()
{

	WORD wVersionRequested;// Para trabajar con sockets en Windows
	WSADATA wsaData;// Para trabajar con sockets en Windows
	SOCKET sockfd,nuevosockfd; //definicion de sockets
	struct sockaddr_in  local_addr,remote_addr; //Estructura basica para llamadas al sistema y funciones relacionada con direcciones de Internet
	char buffer_out[1024],buffer_in[1024], cmd[10], usr[10], pas[10],num1[4],num2[4],copia[17]; //buffers y entrada de datos
	int err,tamanio,i; //numeros enteros para la definicion de errores y el tama�o
	int fin=0, fin_conexion=0;
	int recibidos=0,enviados=0; //inicializacion de las variables recibidos y enviados para el manejo de datos
	int estado=0;
	int numbr1,numbr2, resultado; //variables para la suma
    char* ptr; //puntero para la funcion strtok
    char partes[100][10]; //array para guardar las partes de la funcion strtok
    int n_partes; //n� de partes de la funcion strtok


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


	sockfd=socket(AF_INET,SOCK_STREAM,0);//Creaci�n del socket

	if(sockfd==INVALID_SOCKET)	{ //Si el socket es inv�lido, no se ha creado bien, se sale
		return(-3);
	}
	else {
		local_addr.sin_family		=AF_INET;			// Familia de protocolos de Internet
		local_addr.sin_port			=htons(TCP_SERVICE_PORT);	// Puerto del servidor
		local_addr.sin_addr.s_addr	=htonl(INADDR_ANY);	// Direccion IP del servidor Any cualquier disponible
													// Cambiar para que conincida con la del host
	}
	
	
	//bind: Asocia una direcci�n local con un socket
	// devuelve 0:Proceso de vinculaci�n con el puerto correcto, si no -4 (Error en la vinculaci�n)
	// Enlace el socket a la direccion local (IP y puerto)
	if(bind(sockfd,(struct sockaddr*)&local_addr,sizeof(local_addr))<0)
		return(-4);
	
	//listen:Crea cola de espera para almacenar solicitudes de conexi�n
	//sockfd: socket local enlazado con a un puerto con bind
	//5, N�mero m�ximo de solicitudes de conexi�n en espera mientras el servidor est� ocupado
	//Se prepara el socket para recibir conexiones y se establece el tama�o de cola de espera
	//devuelve 0 si tuvo �xito, si no. -6
	if(listen(sockfd,5)!=0)
		return (-6);
	
	tamanio=sizeof(remote_addr);

	do
	{
		printf ("SERVIDOR> ESPERANDO NUEVA CONEXION DE TRANSPORTE\r\n");
		
		//Accept: Espera una solicitud de conexi�n
		//sockfd: socket en el que se escucha el servidor
		//(struct sockaddr*)&remote_addr: estructura con la direccion del cliente
		//tamanio: tama�o de la estructura de direcci�n del cliente

		//Devuelve: Un entero que representa a un nuevo socket que se usar� para la esa conexi�n o -1(INVALID_SOCKET) si ha habido alg�n fallo
		nuevosockfd=accept(sockfd,(struct sockaddr*)&remote_addr,&tamanio);

		if(nuevosockfd==INVALID_SOCKET) { //Si ha ocurrido algun fallo, devuelve -5
			
			return(-5);
		}

		printf ("SERVIDOR> CLIENTE CONECTADO\r\nSERVIDOR [IP CLIENTE]> %s\r\nSERVIDOR [CLIENTE PUERTO TCP]>%d\r\n",
					inet_ntoa(remote_addr.sin_addr),ntohs(remote_addr.sin_port));

		//Mensaje de Bienvenida
		sprintf_s (buffer_out, sizeof(buffer_out), "%s Bienvenindo al servidor de ECO%s",OK,CRLF);
		
		//Send:Env�a mensaje
		//nuevosocked: Socket local usado para enviar o recibir
		//buffer_out: buffer de datos,
		//(int)strlen(buffer_out): tama�o en bytes del mensaje a enviar
		//0: flag que modifica el comportamiento de la llamada, 0: funcionamiento normal del protocolo
		//Send devuelve La cantidad de bytes enviados o recibidos, 0 si la conexi�n ha sido liberada de forma acordada o SOCKET_ERROR si la operaci�n ha fallado
		enviados=send(nuevosockfd,buffer_out,(int)strlen(buffer_out),0);
		//TODO Comprobar error de env�o
 /*************************************/	
		if(enviados== SOCKET_ERROR) //
		{
					DWORD error=GetLastError();
					
					printf("SERVIDOR> Error %d en la recepcion de datos\r\n",error);
		
					estado=S_QUIT; // Salimos con "estado=S_QUIT"
		}
		else // Si recibimos un 0, la conexion ha sido liberada de forma acordada
		{
					printf("Conexi�n finalizada\r\n");
					estado=S_QUIT;
					fin_conexion=1;
					
		} 
/**************************************/
			
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
			//1024: tama�o maximo en bytes del buffer
			//flags: modifican el comportamiento de la llamada, 0, funcionamiento normal
			//Valores devueltos: La cantidad de bytes enviados o recibidos.
				//0 si la conexi�n ha sido liberada de forma acordada.
				//-1 (o SOCKET_ERROR) si la operaci�n ha fallado
			recibidos = recv(nuevosockfd,buffer_in,1023,0);
			//TODO Comprobar posible error de recepci�n
/************************************************/
			if(recibidos==SOCKET_ERROR || recibidos==0)
						{
							if(recibidos==SOCKET_ERROR)
							{
							DWORD error=GetLastError();
							printf("SERVIDOR> Error %d en la recepcion de datos\r\n",error);
							fin_conexion=1;
							}
							else
							{
							printf("SERVIDOR> Conexi�n cerrada\r\n");
							fin_conexion=1;
							}
						}
/************************************************/		
			buffer_in[recibidos] = 0x00;
			printf ("SERVIDOR [bytes recibidos]> %d\r\nSERVIDOR [datos recibidos]>%s", recibidos, buffer_in);  //mostramos los bytes recibidos y los datos del buffer
			
			switch (estado)
			{
				case S_USER:    /*****************************************/
					strncpy_s ( cmd, sizeof(cmd),buffer_in, 4);
					cmd[4]=0x00; // en C los arrays finalizan con el byte 0000 0000

					if ( strcmp(cmd,SC)==0 ) // si recibido es solicitud de conexion de aplicacion
					{
						sscanf_s (buffer_in,"USER %s\r\n",usr,sizeof(usr));
						
						// envia OK acepta todos los usuarios hasta que tenga la clave
						sprintf_s (buffer_out, sizeof(buffer_out), "%s%s", OK,CRLF);
						
						estado = S_PASS;
						printf ("SERVIDOR> Esperando clave\r\n");
					} else
					if ( strcmp(cmd,SD)==0 ) //Finalizacion de conexion, SD= QUIT
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexi�n%s", OK,CRLF);
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
							sprintf_s (buffer_out, sizeof(buffer_out), "%s Autenticaci�n err�nea%s",ER,CRLF);
							estado=S_USER; //Pasamos al estado S_USER.

						}

					} else
					if ( strcmp(cmd,SD)==0 ) 
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexi�n%s", OK,CRLF);
						fin_conexion=1;
					}
					/*else
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto%s",ER,CRLF);
					}*/
				break;

				case S_DATA: /***********************************************************/
					
					buffer_in[recibidos] = 0x00;
					
					strncpy_s(cmd,sizeof(cmd), buffer_in, 4);
					printf ("SERVIDOR [Comando]>%s\r\n",cmd);
					
					if ( strcmp(cmd,SD)==0 )
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexi�n%s", OK,CRLF);
						fin_conexion=1;
					}
					else if (strcmp(cmd,SD2)==0)
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Finalizando servidor%s", OK,CRLF);
						fin_conexion=1;
						fin=1;
					}
					else if (strcmp(cmd,SUM)==0)
					{
						copia[14]=0x00;
						strncpy_s(copia,sizeof(copia), buffer_in, 14);
					 // Partes en la cadena.
					    n_partes =3;
					  // Primer strtok
					   ptr = strtok(copia, " "); //Filtramos en el primer espacio
					  strcpy(partes[0], ptr);  // copio el contenido en una variable con las partes del strtok, y con el bucle recogemos el resto
  
					 for ( i = 1; i< n_partes; i++)
						{
						   ptr = strtok(NULL," ");
						   strcpy(partes[i], ptr);
						 }
						 numbr1=atoi(partes[1]);
						 numbr2=atoi(partes[2]);

						resultado=numbr1+numbr2;
						if(numbr1<0 || numbr2<0 || numbr1>9999 || numbr2>9999 || numbr1>9999 && numbr2>9999)
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s Se han introducido numeros negativos o mayores de 4 cifras %s",ER,CRLF);
							fin_conexion=1;
						}
						else{
					
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %d %s",OK,resultado,CRLF);
						}
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
			/*****************************************/
			if(enviados==SOCKET_ERROR || enviados==0)
						{
							if(enviados==SOCKET_ERROR)
							{
							DWORD error=GetLastError();
							printf("SERVIDOR> Error %d en el env�o de datos\r\n",error);
							estado=S_QUIT;
							}
							else
							{
							printf("SERVIDOR> Conexi�n cerrada\r\n");
							fin_conexion=1;
							fin=0;
							}
						}
			/*****************************************/

		} while (!fin_conexion);
		printf ("SERVIDOR> CERRANDO CONEXION DE TRANSPORTE\r\n");

		//Shutdown: Deshabilita al recepci�n y/o el env�o de datos por el socket
		//nuevosockfd:descriptor de socket que se quiere modificar su estado
		//how:flag que describe que operaci�n no estar� permitida a partir de la ejecuci�n de la funci�n, en nuestro caso:
		//SD_SEND: Se deshabilita la transmisi�n de datos
		//Valores devueltos: 0 si el socket se ha cerrado correctamente y -1 (SOCKET_ERROR) si la operaci�n ha fallado
		shutdown(nuevosockfd,SD_SEND);


		//Closesocket:Cierra socket
		//nuevosockfd: descriptor del socket que queremos cerrar
		//Devuelve: 0 si el socket se ha cerrado correctamente, o SOCKET_ERROR (-1) si la operaci�n ha fallado 
		closesocket(nuevosockfd);

	}while(!fin);

	printf ("SERVIDOR> CERRANDO SERVIDOR\r\n");

	return(0);
} 
