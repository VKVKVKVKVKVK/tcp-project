#include <ostream>
#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<linux/ip.h>
#include<linux/tcp.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#include "../automates/automate.cpp"

using namespace std;
void print_segment(tcphdr* tcph, unsigned char* buff);

namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}





int main(int argc, char const *argv[])
{
    u_int16_t port = atoi(argv[1]);
   
    int state = 1; //LISTEN 

    // Structs that contain source IP addresses
    struct sockaddr_in source_socket_address, dest_socket_address;

    int packet_size;

    // Allocate string buffer to hold incoming packet data
    unsigned char buffer[65536];
    struct sockaddr_in sin;
    memset(buffer, 0, 65536);
    // Open the raw socket
    int sock = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sock == -1)
    {
        //socket creation failed, may be because of non-root privileges
        perror("Failed to create socket");
        exit(1);
    }
    while(1)
    {
      packet_size = recvfrom(sock , buffer , 65536 , 0 , NULL, NULL);
      if (packet_size == -1) {
        printf("Failed to get packets\n");
        return 1;
      }

      struct iphdr *ip = (struct iphdr *)buffer;
      struct tcphdr *tcp = (struct tcphdr *)(buffer + sizeof(struct iphdr));
      
      //check if dest port is equal to one of server
      if (ntohs(tcp->dest) != port)
      {
	printf("Not the correct port!\n");
	ip = NULL;
	tcp = NULL;
	continue;
      }

      memset(&source_socket_address, 0, sizeof(source_socket_address));
      source_socket_address.sin_addr.s_addr = ip->saddr;
      memset(&dest_socket_address, 0, sizeof(dest_socket_address));
      dest_socket_address.sin_addr.s_addr = ip->daddr;

      print_segment(tcp, buffer);
      

      struct flags flag = automate(state, tcp->syn, tcp->ack, tcp->fin, false);
      
    }

    return 0;
}


void print_segment(tcphdr* tcph, unsigned char* buff){

    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    /*
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
*/

    cout << green << " 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 " << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|        Source Port            |        Destination Port       |" << endl;
    cout << green << "|        ";

    cout << red << ntohs(tcph->source);

    cout << green << "                   |        ";

    cout << red << ntohs(tcph->dest);

    cout << green << "                  |" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|                       Sequence  Number                        |" << endl;
    cout << green << "|                       ";

    cout << red << tcph->seq;

    cout << green <<"                                     |" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|                     Acknowledgment Number                     |" << endl;
    cout << green <<"|                     ";

    cout << red << tcph->ack_seq;

    cout << green <<"                                         |" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green <<"| Offset|  Reserved |   Flags   |             Window            |" << endl;
    cout << green <<"|       |           |U|A|P|R|S|F|                               |" << endl;
    cout << green <<"|       |           |R|C|S|S|Y|I|                               |" << endl;
    cout << green <<"|       |           |G|K|H|T|N|N|                               |" << endl;
    cout << green <<"|   ";

    cout << red << to_string(tcph->doff);

    cout << green << "   |     ";

    cout << red << to_string(tcph->res1);

    cout << green << "     |";

    cout<< red <<to_string(tcph->urg);

    cout << green <<"|";

    cout << red << to_string(tcph->ack);

    cout << green << "|";

    cout << red <<  to_string(tcph->psh);

    cout << green << "|";

    cout << red << to_string(tcph->rst);

    cout << green <<"|";

    cout << red << to_string(tcph->syn);

    cout << green << "|";

    cout << red << to_string(tcph->fin);

    cout << green << "|";

    cout << "             " << red << tcph->window;

    cout << green << "                 |" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|            Checksum           |         Urgent Pointer        |" << endl;
    cout << green <<"|            ";

    cout << red << tcph->check;

    cout << green << "                  |            ";

    cout << red << tcph->urg_ptr;

    cout << green << "                  |" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|                    Options                    |    Padding    |" << endl;
    cout << green << "|                                               |               |" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;



    // cout << buff + sizeof(ip_hdr) << endl;
    cout << green <<"Payload ===>  " ;

    cout << red << buff + sizeof(tcphdr) << endl;
}
