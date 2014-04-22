#include <ws2tcpip.h> 
#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
int fillingTheBuffer(char *buffer);
DWORD WINAPI receivingDatagrams(LPVOID ss);
static int PORT;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int *argc, char **argv)
{
	PORT = atoi(argv[1]);
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		printf ("WSAStartup failed: %d\n", GetLastError ());
		return -1;
	}
	SOCKET s;
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if(!s)
	{
		printf("error socket");
		return -1;
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
	char ac[80];
    gethostname(ac, sizeof(ac));
    hostent *phe = gethostbyname(ac);
	char *ipaddr = inet_ntoa(*((in_addr*)phe->h_addr_list[0]));
///////////////////////////////////////////////////////////////////////////////////////////////////////////
	sockaddr_in sen;
	memset(&sen, 0, sizeof(sen));
	sen.sin_family = AF_INET;
	sen.sin_port = htons(atoi(argv[1]));
	sen.sin_addr.s_addr = inet_addr(ipaddr);
	int error = bind(s, (struct sockaddr*)&sen, sizeof(sen));
	if(error == -1)
	{
		printf("error bind");
		return -1;
	}
	ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr("234.5.6.7");
	mreq.imr_interface.s_addr = INADDR_ANY;
	int t=setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP,(char*)&mreq, sizeof(mreq));
///////////////////////////////////////////////////////////////////////////////////////////////////////////
	sockaddr_in ren;
	memset(&ren, 0, sizeof(ren));
	ren.sin_family = AF_INET;
	ren.sin_port = htons(atoi(argv[1]));
	ren.sin_addr.s_addr = inet_addr("234.5.6.7");
///////////////////////////////////////////////////////////////////////////////////////////////////////////
	char buffer[255];
	DWORD thID;
    CreateThread(NULL, NULL, receivingDatagrams, &s, NULL, &thID);
	while(1)
	{
		int sizeBuffer = fillingTheBuffer(buffer);		
		int sizeSend = sendto(s, buffer, sizeBuffer , 0,(struct sockaddr *)&ren, sizeof(ren));
		if(!sizeSend)
		{
			printf("error send");
		}
	}
}
int fillingTheBuffer(char *buffer)
{
	char ac[80];
    gethostname(ac, sizeof(ac));
    hostent *phe = gethostbyname(ac);
	char *ipaddr = inet_ntoa(*((in_addr*)phe->h_addr_list[0]));
	char nach[255] = "[";
	char cone[8] = "] ---> ";
	char symbol;
	for(int i=0; i<255; i++)
	{
		symbol=getch();
		if(symbol == 13)
		{
			buffer[i] = '\0';
			printf("\r");
			strcat(nach,ipaddr);
			strcat(nach,cone);
			strcat(nach, buffer);
			strcpy(buffer, nach);
			int size = strlen(buffer);
			return size;
		}
		printf("%c",symbol);
		buffer[i]=symbol;
		symbol=NULL;
	}
}
DWORD WINAPI receivingDatagrams(LPVOID ss)
{
	SOCKET mySock;
    mySock = ((SOCKET *)ss)[0];
	char buffer[255];
	sockaddr_in ren;
	ren.sin_family = AF_INET;
    ren.sin_port = htons(PORT);
	ren.sin_addr.s_addr = inet_addr("234.5.6.7");
	int sizeRen[1];
	sizeRen[0] = sizeof(ren);
	while(1)
	{
		ZeroMemory(buffer,sizeof(buffer));
		int sizeRecv = recvfrom(mySock, buffer, sizeof(buffer), 0,(struct sockaddr *)&ren, sizeRen);
		if(!sizeRecv)
		{
			printf("error recv");
		}
		for(int i=0; i<sizeRecv; i++)
		{
			printf("%c",buffer[i]);
		}
		printf("\n");
		sizeRecv = 0;
	}
}