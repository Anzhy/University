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
	char SendData[]="������������ ������ �� ������ ����� 1 - PING";
	char *dest=argv[1];
	int kol=5;
	hostInfo = gethostbyname(dest);
	if(hostInfo == NULL)
	{
		printf("����� � ������ ������� �� ������\n");
		getch();
		return 0;
	}
	memcpy(&tmp.sin_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
	destIp = inet_ntoa(tmp.sin_addr);
	IPAddr ipaddr = inet_addr(destIp);
	hIcmpFile = IcmpCreateFile();																			//C������ ����������, � ������� �� ���������� ��������
	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
	ReplyBuffer = (VOID*) malloc(ReplySize);
	while(kol!=0)
	{                                                                                         
		if(IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000)) //IcmpSendEcho - �������� ����� ������������� ���������� ������
		{                                                                                                   //IcmpHandle - ������������� ����������, �������������� ��� ������ IcmpCreateFile         //RequestOptions - �������������� �������� �������
			PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;									//DestinationAddress - ����� ���������� �����											  //ReplyBuffer - ����� ������ ��� ������ ����������
			printf("�������� icmp ��������� ��� %s  *�������*\n", destIp);									//RequestData - ����� � �������, ������� ���������� ��� �������							  //ReplySize - ������ ������ ������
			printf("���������� ���������� �� ������: %s\n", pEchoReply->Data);								//RequestSize - ������ ������ �������;													  //Timeout - �����, � ������� �������� �� ������� ������ �� �����
			printf("RTT = %ld ����������\n\n\n", pEchoReply->RoundTripTime);								//����� ����� ��������� ������� � ���������� ������
		}
		else 
		{
			printf("�������� icmp ��������� ����������� ��������\n");
			printf("��� ������: %ld\n", GetLastError() );
		} 
		kol--;
	}
	IcmpCloseHandle(hIcmpFile);																				//��������� ����������, � ������� �� ��������
	getch();
	WSACleanup();
	return 0;
}