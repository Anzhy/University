#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")
#define MY_PORT 1234
int main(int argv[], char *argc[])
{
	WSADATA wsd;
    WSAStartup(0X0101, &wsd);
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port   = htons(MY_PORT);
    peer.sin_addr.S_un.S_addr = inet_addr("192.168.1.13"); 
    int rc = connect(s,(struct sockaddr*) &peer, sizeof(peer));
    if (rc<0)
		printf ("error");
///////////////////////////////////////////////////////////////////////////////
    char buffer[1024];
	char infosize[50];
	char cotrserv[1];
    int size=0;
	int position=0;
	int error=0;
	int readinfo=0;
    FILE *in = fopen("C:/client/123.avi","rb");
    fseek(in, 0, SEEK_END);
    size = ftell(in);
	fseek(in, 0, SEEK_SET);
	itoa (size, infosize, 10);
	send(s,infosize,sizeof(infosize),0);
	error=recv( s, infosize, sizeof(infosize), 0 );
	if(error==0||error==-1)
	{
		printf("disconnection\n");
		getch();
		return 0;
	}
	position=atoi(infosize);
	fseek(in, position, SEEK_CUR);
	size-=position;
	while(size)
	{
		readinfo=fread(buffer,1,sizeof(buffer),in);
		if(readinfo!=0)
			send(s,buffer,readinfo,0);
		size-=readinfo;
		error=recv( s,cotrserv,sizeof(char), 0 );
		if(error==0||error==-1)
		{
			printf("disconnection\n");
			getch();
			return 0;
		}
	}
	fclose(in);
	return 0;     
}