#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")
#define MY_PORT 123
DWORD WINAPI ToClient(LPVOID client_socket);
int main()
{
	setlocale(0,"");
	WSADATA BUFFER[400];
	SOCKET mysocket;
	//char buff[1024]; // Буфер для различных нужд
	WSAStartup(0x0202, BUFFER);
	mysocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //0 <-> IPPROTO_TCP
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);
	local_addr.sin_addr.s_addr = 0;//
	bind(mysocket, (sockaddr *)&local_addr, sizeof(local_addr));
	listen(mysocket, 0x100);
	SOCKET client_socket; // сокет для клиента
    sockaddr_in client_addr; // адрес клиента (заполняется системой)
	int client_addr_size = sizeof(client_addr);
	while ((client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size)))
	{
		HOSTENT *hst;
        hst = gethostbyaddr((char *)&client_addr. sin_addr.s_addr, 4, AF_INET);
		DWORD thID;
        CreateThread(NULL, NULL, ToClient, &client_socket, NULL, &thID);
	}
	 return 0;
}


DWORD WINAPI ToClient(LPVOID client_socket)
{
    SOCKET my_sock;
    my_sock = ((SOCKET *)client_socket)[0];
    char buff[20 * 1024];
    int bytes_recv;
	buff[1]='/0';
    while((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && bytes_recv != SOCKET_ERROR)
	{
		int i=0;
		while(i != bytes_recv)
		{
			printf("%c",buff[i]);
			i++;
		}
		if(buff[1]!='0')
		{
			while(1)
			{
				char inf[2];
				//inf[1]='/0';
				*inf=getch();
				if(inf[0]==buff[0])
				{
					printf("\n");
					send(my_sock, &buff[0], 2, 0);
					break;
				}
				printf("%c",inf[0]);
				send(my_sock, &inf[0], 1, 0);
			}
			*buff=NULL;
			buff[1]='/0';
			/*scanf("%s",&buff);
			int sizeBuffer=strlen(buff);
			send(my_sock, &buff[0], sizeBuffer, 0);
			*buff=NULL;
			buff[1]='/0';*/
		}
	}
    closesocket(my_sock);
    return 0;
}