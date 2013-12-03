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
	int size=0;
	FILE *in = fopen("C:/client/123.avi","rb");
	fseek(in, 0, SEEK_END);
	size = ftell(in);
///////////////////////////////////////////////////////
	FILE *error=fopen("C:/client/error.txt","a+");    
	fseek(error, 0, SEEK_END);                      
	int temp=ftell(error);                          
	if(temp==0)                                     
	{
		fseek(in, 0, SEEK_SET);
		char bufcontr[20];
		int nby = recv( s, bufcontr, sizeof(bufcontr), 0 );
		if(nby!=1 && nby!=-1 && nby!=0)
		{
			int sizes=atoi(bufcontr);
			fseek(in, 0, SEEK_SET);
			fseek(in, sizes, SEEK_CUR);
			size=size-sizes;
		}
	}                                                          //   проверка нужна ли докачка
	else                                             
	{
		char bufcontrr[20];
		int nby = recv( s, bufcontrr, sizeof(bufcontrr), 0 );
		fseek(error, 0, SEEK_SET);
		char *tem=new char[temp];
		fread(tem,temp,sizeof(char),error);
		int sizeseek=atoi(tem);
		fseek(in, 0, SEEK_SET);
		fseek(in, sizeseek, SEEK_CUR);
		size=size-sizeseek;
	}                                                
	fclose(error);
	error=fopen("C:/client/error.txt","w");
	fclose(error);
///////////////////////////////////////////////////////
	int b=0;
	char control[1];
	while(size)
	{
		int nbytes = recv(s, control, sizeof(char), 0 );
////////////////////////////////////////////////////////////
		if (nbytes==0||nbytes==-1)                 
		{                                                 
			FILE *error=fopen("C:/client/error.txt","w"); 
			int si=ftell(in);                             
			char err[20];                                  // контроль обрыва соединения
			itoa (si, err, 10);                      
			fwrite(err,1,strlen(err),error);              
			fclose(error);                                
			printf("Disconnect server");				  
			break;
		}                                                 
////////////////////////////////////////////////////////////
		b=fread(buffer,1,sizeof(buffer),in);
		size-=b;
		if(b!=0)
			send(s,buffer,b,0);
	}
	Sleep(1000);
	fclose(in);
	return 0;
}