#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")
#define MY_PORT 1234
DWORD WINAPI ToClient(LPVOID client_socket);
int main(int argv[], char *argc[])
{
	setlocale(0,"");
	WSADATA BUFFER[400];
	WSAStartup(0x0202, BUFFER);
	SOCKET mysocket;
	mysocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //0 <-> IPPROTO_TCP
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);
	local_addr.sin_addr.s_addr = 0;//
	bind(mysocket, (sockaddr *)&local_addr, sizeof(local_addr));
	listen(mysocket, 0x100);
	SOCKET client_socket; // сокет для клиента
    sockaddr_in client_addr; // адрес клиента (заполняется системой)
///////////////////////////////////////////////////////////////////////////////
	int client_addr_size = sizeof(client_addr);
	while ((client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size)))
	{
		DWORD thID;
        CreateThread(NULL, NULL, ToClient, &client_socket, NULL, &thID);
	}
	 return 0;
}
///////////////////////////////////////////////////////////////////////////////
DWORD WINAPI ToClient(LPVOID client_socket)
{
    SOCKET my_sock;
    my_sock = ((SOCKET *)client_socket)[0];
	char buf[1024];
	int sizeseek=0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FILE *error=fopen("C:/server/error.txt","a+");    
	fseek(error, 0, SEEK_END);                       
	int temp=ftell(error);
	if(temp==0)
	{
		send(my_sock,"*",sizeof(char),0);
	}
	else
	{
		fseek(error, 0, SEEK_SET);
		char *tem=new char[temp];
		fread(tem,temp,sizeof(char),error);
		sizeseek=atoi(tem);
		send(my_sock,tem,temp,0);
	}
	fclose(error); 
	error=fopen("C:/server/error.txt","w");
	fclose(error);
	while (1)
	{
		FILE *in = fopen("C:/server/123.avi","ab");
		int control=send(my_sock,"*",sizeof(char),0);
		int nbytes = recv( my_sock, buf, sizeof(buf), 0 );
	    if (nbytes==0||nbytes==-1)                     
		{
			fseek(in, 0, SEEK_END);
			int temps=ftell(in);
			FILE *error=fopen("C:/server/error.txt","w");
			char err[20];                                      // контроль обрыва соединения
			itoa (temps, err, 10);               
			fwrite(err,1,strlen(err),error);              
			fclose(error);              
			fclose(in);
			printf("Disconnect client");				  
			break;
		}          
		fwrite(buf,1,nbytes,in);
		fclose(in);
	}
	return 0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}