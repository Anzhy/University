#include <stdio.h>
#include <conio.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
WORD InternetChksum(LPWORD lpwIcmpData, WORD wDataLength);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// В RFC 792 определены значения сообщений ICMP
#define ICMP_ECHO 8			// An ICMP echo message
#define ICMP_ECHOREPLY 0		// An ICMP echo reply message
#define	ICMP_HEADERSIZE 8
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
struct icmp				// Структура заголовка ICMP
{
	BYTE icmp_type;			// Type of message
	BYTE icmp_code;			// Type "sub code" (zero for echos)
	WORD icmp_cksum;		// 1's complement checksum
	HINSTANCE icmp_id;		// Unique ID (the instance handle)
	WORD icmp_seq;			// Tracks multiple pings
	BYTE icmp_data[1];		// The start of optional data
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int *argc, char **argv)
{
	WSADATA wsaData;
	WSAStartup(0x0101, &wsaData);

	SOCKADDR_IN	sockAddrLocal;	// Структуры адреса сокета
	SOCKADDR_IN	sockAddrHost;	// 
	SOCKET hSocket;			    // Дескриптор сокета

	LPHOSTENT lpHostEntry;		// Структура данных с информацией о сетевом компьютере
	LPPROTOENT lpProtocolEntry;	// Структура данных с информацией о протоколе

	BYTE IcmpSendPacket[1024];	// Буфер для посылаемых данных
	BYTE IcmpRecvPacket[4096];	// Буфер для принимаемых данных 

	icmp *pIcmpHeader;	// Указатель на структуру ICMP

	int iSentBytes;			// Количество посланных байтов
	int iReceivedBytes;		// Количество принятых байтов
	int iPacketSize;		// размер ICMP-пакета 
	int iIPHeadLength;		// Длина заголовка IP-датаграммы
	PDWORD pdwTimeStamp;	// Счетчик при передаче
	DWORD dwReturnTime;		// Счетчик при приеме
	DWORD dwRoundTrip;		// Счетчик среднего времени
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if ((lpHostEntry = gethostbyname(argv[1])) == NULL) // Получает информацию о хосте по его имени. Результат работы помещается в специальную структуру lpHostEntry
	{
		printf("Could not get %s IP address.\n", (LPSTR)argv[1]);
		return 0;
	}

	sockAddrLocal.sin_family = AF_INET;
	sockAddrLocal.sin_addr = *((LPIN_ADDR) *lpHostEntry->h_addr_list); // Пишеи адрес из lpHostEntry в sockAddrLocal
	if ((hSocket = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) == INVALID_SOCKET) // Создаем RAW сокет и указываем ICMP в качестве протокола
	{
		printf("error socket\n");
		return 0;
	}

	pIcmpHeader = (struct icmp *) IcmpSendPacket;	// Данные
	pIcmpHeader->icmp_type = ICMP_ECHO;		// Эхо-запрос =8 
	pIcmpHeader->icmp_code = 0;	 
	pIcmpHeader->icmp_id = 0;
	pIcmpHeader->icmp_seq = 0;
	pIcmpHeader->icmp_cksum = 0;

	//Значение счетчика "тиков" располагается в необязательной области данных
	pdwTimeStamp = (PDWORD)&IcmpSendPacket[ICMP_HEADERSIZE];
	*pdwTimeStamp = GetTickCount();
	iPacketSize = ICMP_HEADERSIZE + sizeof(DWORD); // Количество посылаемых байт
	pIcmpHeader->icmp_cksum = InternetChksum((LPWORD)pIcmpHeader, iPacketSize); // Записываем чек-сумму в структуру ICMP
  
	iSentBytes = sendto(hSocket, (LPSTR)IcmpSendPacket, iPacketSize, 0, (LPSOCKADDR)&sockAddrLocal, sizeof(sockAddrLocal)); // Посылаем
	if (iSentBytes == SOCKET_ERROR) 
	{
		closesocket(hSocket);
		printf("error\n");
		return 0;
	}

	int iHostAddrLength = sizeof(sockAddrHost);
	iReceivedBytes = recvfrom(hSocket, (LPSTR)IcmpRecvPacket, sizeof(IcmpRecvPacket), 0, (LPSOCKADDR)&sockAddrHost, &iHostAddrLength); // Принимаем
	closesocket(hSocket); // Закрываем RAW сокет

	dwReturnTime = GetTickCount();
	dwRoundTrip = dwReturnTime - *pdwTimeStamp; // Определяем время за прохождение информации туда и обратно (RTT)

	printf("Round-trip travel time (RTT) to %s [%s] was %d milliseconds.\n",(LPSTR)argv[1], (LPSTR)inet_ntoa(sockAddrHost.sin_addr), dwRoundTrip);
	getch();
	return(1);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WORD InternetChksum(LPWORD lpwIcmpData, WORD wDataLength)
{
	long	lSum;		// Store the summation
	WORD	wOddByte;	// Left over byte from the summation
	WORD	wAnswer;	// The 1's complement checksum
		
	lSum = 0L;

	while (wDataLength > 1)
		{
			lSum += *lpwIcmpData++;
			wDataLength -= 2;
		}

	// Handle the odd byte if necessary and make sure the top half is zero
	if (wDataLength == 1)
		{
			wOddByte = 0;
			*((LPBYTE) &wOddByte) = *(LPBYTE)lpwIcmpData;	// One byte only
			lSum += wOddByte;
		}

	// Add back the carry outs from the 16 bits to the low 16 bits
	lSum = (lSum >> 16) + (lSum & 0xffff);	// Add high-16 to low-16
	lSum += (lSum >> 16);			// Add carry
	wAnswer = (WORD)~lSum;		// 1's complement, then truncate 
					// to 16 bits
	return(wAnswer);
}