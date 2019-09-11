//#include <winsock.h>
#include <iostream>
#include <WinSock2.h>
//#include <stdint.h>
#include <ws2tcpip.h>
#include <conio.h>


#pragma comment(lib, "Ws2_32.lib")

#define PACKET_MAX_SIZE 1523 //FIXME

using namespace std;

typedef struct ip_hdr
{
    unsigned char ip_header_len : 4; // 4-bit header length (in 32-bit words) normally=5 (Means 20 Bytes may be 24 also)
    unsigned char ip_version : 4; // 4-bit IPv4 version
    unsigned char ip_tos; // IP type of service
    unsigned short ip_total_length; // Total length
    unsigned short ip_id; // Unique identifier
    unsigned char ip_frag_offset : 5; // Fragment offset field
    unsigned char ip_more_fragment : 1;
    unsigned char ip_dont_fragment : 1;
    unsigned char ip_reserved_zero : 1;
    unsigned char ip_frag_offset1; //fragment offset
    unsigned char ip_ttl; // Time to live
    unsigned char ip_protocol; // Protocol(TCP,UDP etc)
    unsigned short ip_checksum; // IP checksum
    unsigned int ip_srcaddr; // Source address
    unsigned int ip_destaddr; // Source address
} IPV4_HDR, *PIPV4_HDR, FAR * LPIPV4_HDR;

typedef struct tcp_header
{
    unsigned short source_port; // source port
    unsigned short dest_port; // destination port
    unsigned int sequence; // sequence number - 32 bits
    unsigned int acknowledge; // acknowledgement number - 32 bits
    unsigned char ns : 1; //Nonce Sum Flag Added in RFC 3540.
    unsigned char reserved_part1 : 3; //according to rfc
    unsigned char data_offset : 4; /*The number of 32-bit words in the TCP header.
    This indicates where the data begins.
    The length of the TCP header is always a multiple
    of 32 bits.*/
    unsigned char fin : 1; //Finish Flag
    unsigned char syn : 1; //Synchronise Flag
    unsigned char rst : 1; //Reset Flag
    unsigned char psh : 1; //Push Flag
    unsigned char ack : 1; //Acknowledgement Flag
    unsigned char urg : 1; //Urgent Flag
    unsigned char ecn : 1; //ECN-Echo Flag
    unsigned char cwr : 1; //Congestion Window Reduced Flag

    ////////////////////////////////

    unsigned short window; // window
    unsigned short checksum; // checksum
    unsigned short urgent_pointer; // urgent pointer
} TCP_HDR, *PTCP_HDR, FAR * LPTCP_HDR, TCPHeader, TCP_HEADER;

void print_segment(tcp_header* tcph, char* buff){
    cout << "Source Port: "  << tcph->source_port << endl;
    cout << "Dest Port: "  << tcph->dest_port << endl;
    cout << "CWR: "  << to_string(tcph->cwr) << endl;
    cout << "ECN: "  << to_string(tcph->ecn) << endl;
    cout << "URG: "  << to_string(tcph->urg) << endl;
    cout << "ACK: "  << to_string(tcph->ack) << endl;
    cout << "PSH: "  << to_string(tcph->psh) << endl;
    cout << "RST: "  << to_string(tcph->rst) << endl;
    cout << "SYN: "  << to_string(tcph->syn) << endl;
    cout << "FIN: "  << to_string(tcph->fin) << endl;
    cout << "NS: "  << to_string(tcph->ns) << endl;
    cout << "Sequence: "  << tcph->sequence << endl;
    cout << "ACKNOWLEDGE: "  << tcph->acknowledge << endl;
    cout << "Reserved Part1: "  << to_string(tcph->reserved_part1) << endl;
    cout << "Data offset: "  << to_string(tcph->data_offset) << endl;
    cout << "Window: "  << tcph->window << endl;
    cout << "Checksum: "  << tcph->checksum << endl;
    cout << "Urgent Pointer: "  << tcph->urgent_pointer << endl;

    // cout << buff + sizeof(ip_hdr) << endl;
    cout << "Payload: " << buff + sizeof(ip_hdr) + sizeof(tcp_header) << endl;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int red = 12;
    int green = 10;
    int black = 16;
    SetConsoleTextAttribute(hConsole, green);
    cout << " 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 " << endl;
    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << "|        Source Port            |        Destination Port       |" << endl;
    cout << "|        ";
    SetConsoleTextAttribute(hConsole, red);
    cout << tcph->source_port;
    SetConsoleTextAttribute(hConsole, green);
    cout <<"                   |        ";
    SetConsoleTextAttribute(hConsole, red);
    cout<< tcph->dest_port ;
    SetConsoleTextAttribute(hConsole, green);
    cout<< "                  |" << endl;
    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << "|                       Sequence  Number                        |" << endl;
    cout << "|                       ";
    SetConsoleTextAttribute(hConsole, red);
    cout << tcph->sequence;
    SetConsoleTextAttribute(hConsole, green);
    cout <<"                                     |" << endl;
    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << "|                     Acknowledgment Number                     |" << endl;
    cout << "|                     ";
    SetConsoleTextAttribute(hConsole, red);
    cout << tcph->acknowledge;
    SetConsoleTextAttribute(hConsole, green);
    cout <<"                                         |" << endl;
    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << "| Offset|  Reserved |   Flags   |             Window            |" << endl;
    cout << "|       |           |U|A|P|R|S|F|                               |" << endl;
    cout << "|       |           |R|C|S|S|Y|I|                               |" << endl;
    cout << "|       |           |G|K|H|T|N|N|                               |" << endl;
    cout << "|   ";
    SetConsoleTextAttribute(hConsole, red);
    cout << to_string(tcph->data_offset);
    SetConsoleTextAttribute(hConsole, green);
    cout <<"   |     ";
    SetConsoleTextAttribute(hConsole, red);
    cout << to_string(tcph->reserved_part1);
    SetConsoleTextAttribute(hConsole, green);
    cout <<"     |";
    SetConsoleTextAttribute(hConsole, red);
    cout<<to_string(tcph->urg);
    SetConsoleTextAttribute(hConsole, green);
    cout <<"|";
    SetConsoleTextAttribute(hConsole, red);
    cout << to_string(tcph->ack);
    SetConsoleTextAttribute(hConsole, green);
    cout <<"|";
    SetConsoleTextAttribute(hConsole, red);
    cout <<to_string(tcph->psh);
    SetConsoleTextAttribute(hConsole, green);
    cout <<"|";
    SetConsoleTextAttribute(hConsole, red);
    cout<<to_string(tcph->rst);
    SetConsoleTextAttribute(hConsole, green);
    cout <<"|";
    SetConsoleTextAttribute(hConsole, red);
    cout << to_string(tcph->syn);
    SetConsoleTextAttribute(hConsole, green);
    cout <<"|";
    SetConsoleTextAttribute(hConsole, red);
    cout << to_string(tcph->fin);
    SetConsoleTextAttribute(hConsole, green);
    cout <<"|";
    SetConsoleTextAttribute(hConsole, red);
    cout <<"             "<< tcph->window;
    SetConsoleTextAttribute(hConsole, green);
    cout <<"                 |" << endl;
    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << "|            Checksum           |         Urgent Pointer        |" << endl;
    cout << "|            ";
    SetConsoleTextAttribute(hConsole, red);
    cout <<tcph->checksum;
    SetConsoleTextAttribute(hConsole, green);
    cout <<"                  |            ";
    SetConsoleTextAttribute(hConsole, red);
    cout << tcph->urgent_pointer;
    SetConsoleTextAttribute(hConsole, green);
    cout <<"                  |" << endl;
    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << "|                    Options                    |    Padding    |" << endl;
    cout << "|                                               |               |" << endl;
    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;

    SetConsoleTextAttribute(hConsole, black);
}

int main()
{
    WSADATA WSAData;
    SOCKET sock;
    char buff[PACKET_MAX_SIZE] = { 0 };
    WSADATA wsock;
    if (WSAStartup(MAKEWORD(2, 2), &wsock) != 0)
    {
        fprintf(stderr, "WSAStartup() failed");
        exit(EXIT_FAILURE);
    }
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    sockaddr_in dest{ 0 };
   // InetPtonW(AF_INET, L"0.0.0.0", &dest.sin_addr.s_addr);
    dest.sin_addr.s_addr=inet_addr("0.0.0.0");
    dest.sin_family = AF_INET;
    dest.sin_port = 0;
    bind(sock, (sockaddr*)&dest, sizeof(dest));
    //cout << "sock: " << sock << endl;
    cout << "Waiting for connections ..." << endl;
    //system("PAUSE");

    for (;!_kbhit();)
    {
        int bytesIn = recv(sock, buff, 1000, 0);
        if (bytesIn == SOCKET_ERROR)
        {
            cout << "Error Receving from Server" << WSAGetLastError() << endl;
        }
        else
        {

            //cout << "MESSAGE RECV from Server " << " : " << bytesIn << endl;
            tcp_header* tcph = (tcp_header*)(buff + sizeof(ip_hdr));

            print_segment(tcph, buff);

        }
    }
    closesocket(sock);
    WSACleanup();
}