#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>
#include <conio.h>
#include <locale.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
int main(int *argc, char *argv[]) 
{
	setlocale(0, "Russian");
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	HANDLE hIcmpFile;
	LPVOID ReplyBuffer = NULL;
	DWORD ReplySize;
	sockaddr_in tmp;
	hostent *hostInfo;
	char *destIp;
	char SendData[]="Лабораторная работа по СПОЛКС номер 1 - PING";
	char *dest=argv[1];
	int kol=5;
	hostInfo = gethostbyname(dest);
	if(hostInfo == NULL)
	{
		printf("Домен с данным адресом не найден\n");
		getch();
		return 0;
	}
	memcpy(&tmp.sin_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
	destIp = inet_ntoa(tmp.sin_addr);
	IPAddr ipaddr = inet_addr(destIp);
	hIcmpFile = IcmpCreateFile();																			//Cоздает соединение, с которым мы собираемся работать
	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
	ReplyBuffer = (VOID*) malloc(ReplySize);
	while(kol!=0)
	{                                                                                         
		if(IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000)) //IcmpSendEcho - посылает через установленное соединение данные
		{                                                                                                   //IcmpHandle - идентификатор соединения, установленного при помощи IcmpCreateFile         //RequestOptions - дополнительные свойства запроса
			PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;									//DestinationAddress - адрес пингуемого хоста											  //ReplyBuffer - адрес буфера для приема результата
			printf("Отправка icmp сообщения для %s  *УСПЕШНА*\n", destIp);									//RequestData - буфер с данными, которые посылаются при запросе							  //ReplySize - размер буфера приема
			printf("Полученная информация из ответа: %s\n", pEchoReply->Data);								//RequestSize - размер буфера запроса;													  //Timeout - время, в течение которого мы ожидаем ответа от хоста
			printf("RTT = %ld милисекунд\n\n\n", pEchoReply->RoundTripTime);								//Время между отправкой запроса и получением ответа
		}
		else 
		{
			printf("Отправка icmp сообщения закончилась неудачей\n");
			printf("Код ошибки: %ld\n", GetLastError() );
		} 
		kol--;
	}
	IcmpCloseHandle(hIcmpFile);																				//Закрывает соединение, с которым мы работали
	getch();
	WSACleanup();
	return 0;
}