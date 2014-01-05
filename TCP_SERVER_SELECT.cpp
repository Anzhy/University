#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
#define MY_PORT 1234
DWORD WINAPI ToClient(LPVOID client_socket);
static int scet=0;
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
	std::vector<int> listing;
	std::vector<int> sezese;
	std::vector<int> soc;
	int sizeseek=0;
	int size=0;
	int control=0;
	for (;;) 
	{   
		int nsock;                /* дескриптор, полученный с по-мощью accept  */            
		int retour;               /* возвращаемое значение  */ 
		fd_set readf;             /* переменная для select */ 
		struct timeval to;        /* тайм-аут для select  */
		FD_ZERO(&readf);
        FD_SET(mysocket, &readf);   
		to.tv_sec=10;
		retour = select(mysocket+1, &readf, NULL, NULL, &to);
		if (FD_ISSET(mysocket, &readf))
		{
			DWORD thID;
			nsock = accept(mysocket, (struct sockaddr *) 0, (int *)0);
			soc.push_back(nsock);
			listing.push_back(0);
		}
		for(int i=0; i<soc.size(); i++)
		{	
			FILE *in;
			char buf[1024];
			char infosize[50];
			char namefil[10];
			char filename[20]="C:/server/";
			itoa(soc[i],namefil,10);
			strcat(filename,namefil);
			strcat(filename,".avi\0");
			if(listing[i]==0)
			{
				control=recv( soc[i], infosize, sizeof(infosize), 0 );
				if(control==0||control==-1)
				{ 
					printf("disconnection\n");
					return 0;
				}
				sizeseek=atoi(infosize);
				in = fopen(filename,"ab");
				fseek(in, 0, SEEK_END);
				size = ftell(in);
				fseek(in, 0, SEEK_SET);
				fclose(in);
				itoa (size, infosize, 10);
				send(soc[i],infosize,sizeof(infosize),0);
				sizeseek-=size;
				sezese.push_back(sizeseek);
				listing[i]=1;
			}
			if(sezese[i])
			{
				control=recv( soc[i], buf, sizeof(buf), 0 );
				if(control==0||control==-1)
				{
					printf("disconnection\n");
					return 0;
				}
				in = fopen(filename,"ab");
				fwrite(buf,1,control,in);
				fclose(in);
				sezese[i]=sezese[i]-control;
				send(soc[i],"*",sizeof(char),0);
			}
			else
			{
				printf("ЖОПА");
				listing.erase(listing.begin()+i);
				sezese.erase(sezese.begin()+i);
				soc.erase(soc.begin()+i);
			}
		}
	}
	return 0;
}