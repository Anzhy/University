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
    mysocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(MY_PORT);
    local_addr.sin_addr.s_addr = 0;//IP
    bind(mysocket, (sockaddr *)&local_addr, sizeof(local_addr));
    listen(mysocket, 0x100);
    SOCKET client_socket; 
    sockaddr_in client_addr;
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
	char infosize[50];
	int sizeseek=0;
	int size=0;
	int control=0;
	u_long OOB = 1;
	control=recv( my_sock, infosize, sizeof(infosize), 0 );
	if(control==0||control==-1)
	{ 
		printf("disconnection\n");
		return 0;
	}
	sizeseek=atoi(infosize);
	FILE *in = fopen("C:/server/123.avi","ab");
	fseek(in, 0, SEEK_END);
    size = ftell(in);
	fseek(in, 0, SEEK_SET);
	fclose(in);
	itoa (size, infosize, 10);
	send(my_sock,infosize,sizeof(infosize),0);
	sizeseek-=size;
	while(sizeseek)
	{
		control=recv( my_sock, buf, sizeof(buf), 0 );
		if(control==0||control==-1)
		{
			printf("disconnection\n");
			return 0;
		}
		in = fopen("C:/server/123.avi","ab");
		fwrite(buf,1,control,in);
		fclose(in);
		sizeseek-=control;
		send(my_sock,"*",sizeof(char),0);
///////////////////////////////////////////////////////////////////////////////
		ioctlsocket(my_sock, SIOCATMARK, &OOB);
		if(!OOB)
		{
			/*int xuy=recv(my_sock, infosize, sizeof(infosize), MSG_OOB);
			printf("Out of band data:%s                  adopted\n",infosize);*/
			recv(my_sock, infosize, sizeof(infosize), MSG_OOB);
			printf("Out of band data:%c                  adopted\n",infosize[0]);
		}
///////////////////////////////////////////////////////////////////////////////
	}
	return 0;
}