#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
USHORT checksum(USHORT *buffer, int size);
///////////////////////////////////////////////////////////////////////////////////////////////
typedef struct ip_hdr
{
    unsigned char   h_len:4;
    unsigned char   ver:4;
    unsigned char   tos;
    unsigned short  total_len;
    unsigned short  ident;
    unsigned short  frag_and_flags;
    unsigned char   ttl;
    unsigned char   proto;
    unsigned short  checksum;
    unsigned int    sourceIP;
    unsigned int    destIP;
}IP_HDR;
typedef struct icmp_hdr
{
    unsigned char  icmp_type;        
    unsigned char  icmp_code;        
    unsigned short icmp_cksum;      
    unsigned short icmp_id;          
    unsigned short icmp_seq;        
} ICMPHDR;
///////////////////////////////////////////////////////////////////////////////////////////////
#define ICMP_ECHO   8      // Код эхо
#define MAX_PACKETSND   150    // Размер буфера ICMP - пакета
#define IP_HDRINCL 2
///////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	WSADATA wsd;
    SOCKET s;
    DWORD bOpt;
    int ret;
    // IP и ICMP заголовки
    IP_HDR ipHdr;
    ICMPHDR icmpHdr;
    struct sockaddr_in remote;
    unsigned short iTotalSize;
	unsigned short iIPSize;
	unsigned short iIcmpSize;
    char buf[MAX_PACKETSND];
    char *ptr;
	char szPacket[] = "Hello!!!";  // Сообщение, данные ICMP - пакета
	char dwMyIP[]     = "131.131.131.0";   // IP - адрес отправителя
	char dwServerIP[] = "93.84.114.78"; // IP - адрес получателя
///////////////////////////////////////////////////////////////////////////////////////////////
    if (WSAStartup (MAKEWORD(2,2), &wsd) != 0)
	{
        printf ("error");
        return 0;
    }
	s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (s == INVALID_SOCKET)
	{
        printf ("error socket");
        return 0;
    }
    bOpt = 1;
    ret = setsockopt (s, IPPROTO_IP, IP_HDRINCL, (char *)&bOpt, sizeof (bOpt));
    if (ret == SOCKET_ERROR)
	{
        printf ("error");
        return 0;
    }  
///////////////////////////////////////////////////////////////////////////////////////////////
    iTotalSize = sizeof (IP_HDR) + sizeof (ICMPHDR) + strlen (szPacket);
    iIPSize = sizeof (IP_HDR) / sizeof (unsigned long);
///////////////////////////////////////////////////////////////////////////////////////////////
    ipHdr.ver = 4;  //версия протокола -IPv4
    ipHdr.h_len = iIPSize; //длинна заголовка
    ipHdr.tos = 0;
    ipHdr.total_len = htons (iTotalSize); //вызовем функцию htons - тк нам нужен сетевой порядок байт
    ipHdr.ident = 0;
    ipHdr.frag_and_flags = 0;
    ipHdr.ttl = 255; //максимум возможно обойти 255 маршрутизаторов
    ipHdr.proto = IPPROTO_ICMP; //в качестве протокола верхнего уровня будет использован ICMP
    ipHdr.checksum = 0; //изначально поле контрольной суммы полагают равным 0
    ipHdr.sourceIP = inet_addr(dwMyIP); //IP адрес отправителя
    ipHdr.destIP = inet_addr(dwServerIP); //IP адрес получателя
    ipHdr.checksum  = checksum((unsigned short *)&ipHdr, sizeof(IP_HDR));
///////////////////////////////////////////////////////////////////////////////////////////////
    icmpHdr.icmp_type = ICMP_ECHO;  
    icmpHdr.icmp_cksum = 0;
    icmpHdr.icmp_code = 0;
    icmpHdr.icmp_seq = 11;
    icmpHdr.icmp_id = 123;
    iIcmpSize = sizeof (ICMPHDR) + strlen (szPacket);
    ZeroMemory (buf, MAX_PACKETSND);
    ptr = buf;
    memcpy (ptr, &icmpHdr, sizeof (icmpHdr));
    ptr += sizeof (icmpHdr);
    memcpy (ptr, szPacket, strlen (szPacket));
    icmpHdr.icmp_cksum = checksum ((unsigned short *)buf, iIcmpSize);
///////////////////////////////////////////////////////////////////////////////////////////////
    ZeroMemory (buf, MAX_PACKETSND);
    ptr = buf;
    memcpy (ptr, &ipHdr, sizeof (ipHdr));
    ptr += sizeof (ipHdr);
    memcpy (ptr, &icmpHdr, sizeof (icmpHdr));
    ptr += sizeof (icmpHdr);
    memcpy (ptr, szPacket, strlen (szPacket));
///////////////////////////////////////////////////////////////////////////////////////////////
    remote.sin_family = AF_INET;
    remote.sin_port = htons (0);
    remote.sin_addr.s_addr = inet_addr(dwServerIP);
///////////////////////////////////////////////////////////////////////////////////////////////
    ret = sendto (s, buf, iTotalSize, 0, (SOCKADDR *)&remote, sizeof (remote));
    if (ret == SOCKET_ERROR)
	{
        printf ("sendto: %d\n", WSAGetLastError ());
		/*error 10004 Interrupted function call (Прерван вызов функции).
		Блокирующая операция прервана вызовом WSACancelBlockingCall().*/
    }
    closesocket (s);
    WSACleanup ();
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////
USHORT checksum(USHORT *buffer, int size)
{
    unsigned long cksum=0;
    while (size > 1){
        cksum += *buffer++;
        size  -= sizeof(USHORT);  
    }
    if (size){
        cksum += *(UCHAR*)buffer;  
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (USHORT)(~cksum);
}