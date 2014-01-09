#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")
#define MY_PORT 1234
int main(int argv[], char *argc[])
{
	char buff[1030];
	WSADATA wsaData;
	WORD VersionRequested = MAKEWORD(2,2);
    int err = WSAStartup(VersionRequested, &wsaData);
	if (err!=0) 
    {
      printf("error\n");
    }
	SOCKET my_sock=socket(AF_INET, SOCK_DGRAM, 0);
    if (my_sock==-1)
    {
      printf("error\n");
    }
	sockaddr_in dest_addr;
    dest_addr.sin_family=AF_INET;
    dest_addr.sin_port=htons(MY_PORT);
    dest_addr.sin_addr.s_addr=inet_addr("192.168.1.13");
	while(1)
    {
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
		sockaddr_in server_addr;
		int server_addr_size=sizeof(server_addr);
		sendto(my_sock, infosize, sizeof(infosize), 0, (sockaddr *)&dest_addr, sizeof(dest_addr));
		error=recvfrom(my_sock, infosize, sizeof(infosize), 0, (sockaddr *) &server_addr, &server_addr_size);
		if(error==0||error==-1)
		{
			printf("error\n");
			getch();
			return 0;
		}
		position=atoi(infosize);
		fseek(in, position, SEEK_CUR);
		size-=position;
		int schet=0;                                                                                          
		char prover[6]="$$$$$";                                                                              
		while(size)
		{
			readinfo=fread(buff,1,sizeof(buff)-6,in);
			itoa(schet, prover, 10);                                                                              
			int cont=strlen(prover);                                                                              
			if(cont<6)                                                                                            
			{                                                                                                     
				prover[cont]='$';                                                                                 
			}                                                                                                     
			int l=0;                                                                                              
			for(int i=readinfo; i<readinfo+6; i++)                                                                
			{                                                                                                     
				buff[i]=prover[l];                                                                                
				l++;                                                                                              
			}                                                                                                     
			if(readinfo!=0)
				sendto(my_sock, buff, readinfo+6, 0, (sockaddr *)&dest_addr, sizeof(dest_addr));
			char conn[6];                                                                                         
			error=recvfrom(my_sock, conn, sizeof(char)*6, 0, (sockaddr *)&server_addr, &server_addr_size);        
			if(conn[0]!='*')                                                                                      
			{                                                                                                     
				for(int i=0;i<6;i++)                                                                              
				{                                                                                                 
					if(conn[i]=='$')                                                                              
					{                                                                                             
						conn[i]='\0';                                                                             
						break;                                                                                    
					}                                                                                             
				}                                                                                                 
				int rez=atoi(conn);                                                                               
				size=size-((rez*1024)+readinfo);                                                                  
				fseek(in, rez*1024, SEEK_CUR);                                                                    
				schet=rez;                                                                                        
			}                                                                                                     
			size-=readinfo;
			error=recvfrom(my_sock, cotrserv, sizeof(char), 0, (sockaddr *)&server_addr, &server_addr_size);
			if(error==0||error==-1)
			{
				printf("error\n");
				getch();
				return 0;
			}
		}
		fclose(in);
		closesocket(my_sock);
		WSACleanup();
		return 0;
    }
}