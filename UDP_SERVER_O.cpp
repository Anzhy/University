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
        if (err !=0) 
    {
      printf("error\n");
    }
        SOCKET my_sock = socket(AF_INET,SOCK_DGRAM,0);
    if (my_sock==-1)
    {
      printf("error\n");
    }
        sockaddr_in local_addr;
    local_addr.sin_family=AF_INET;
        local_addr.sin_port=htons(MY_PORT);
    local_addr.sin_addr.s_addr=0;//htonl(INADDR_ANY);
    if (bind(my_sock,(sockaddr *) &local_addr, sizeof(local_addr))==SOCKET_ERROR)
    {
      printf("error\n");
      closesocket(my_sock);
    }
        while(1)                                                                                                        
        {
                char infosize[50];
                int sizeseek=0;
                int size=0;
                int control=0;
                sockaddr_in client_addr;                                                                                                
                int client_addr_size = sizeof(client_addr);                                        
                control=recvfrom(my_sock, infosize, sizeof(infosize), 0, (sockaddr *)&client_addr, &client_addr_size);
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
                sendto(my_sock, infosize, sizeof(infosize), 0, (sockaddr *)&client_addr, sizeof(client_addr));
                sizeseek-=size;
				int schet=0;
                while(sizeseek)
                {
                        control=recvfrom(my_sock, buff, sizeof(buff), 0, (sockaddr *)&client_addr, &client_addr_size);
						if(control==0||control==-1)
                        {
                                printf("error\n");
                                return 0;
                        }
						int l=0;
						char con[6];
						for(int i=control-6; i<control; i++)
						{
							if(buff[i]!='$')
								con[l]=buff[i];
							else
							{
								con[l]='\0';
								break;
							}
							l++;
						}
						int rez=atoi(con);
						char conn[6]="$$$$$";
						if(rez!=schet)
						{
							itoa(schet, conn, 10);
							if(strlen(conn)<6)
							{
								conn[strlen(conn)]='$';
							}
							sendto(my_sock, conn, sizeof(char)*6, 0, (sockaddr *)&client_addr, sizeof(client_addr));
						}
						else
						{
							conn[0]='*';
							sendto(my_sock, conn, sizeof(char)*6, 0, (sockaddr *)&client_addr, sizeof(client_addr));
						}
                        in = fopen("C:/server/123.avi","ab");
                        fwrite(buff,1,control-6,in);
                        fclose(in);
                        sizeseek-=control-6;
                        sendto(my_sock, "*", sizeof(char), 0, (sockaddr *)&client_addr, sizeof(client_addr));
                }
                closesocket(my_sock);
                WSACleanup();
                return 0;
        }
}