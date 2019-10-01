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
void centerstring(string s, int field);

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

void centerstring(string s, int field)
{
    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    int l=s.length();
    int pos=(int)((field-l)/2);
    for(int i=0;i<pos;i++)
        cout<<" ";

    cout<< red <<s;
    for(int i=0;i<pos;i++)
        cout<<" ";
    if (s.length()%2 == 0) {
        cout << " ";
    }
}

void print_segment(tcphdr* tcph, unsigned char* buff){

    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    cout << green << " 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 " << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|        Source Port            |        Destination Port       |" << endl;
    cout << green << "|";

    centerstring(std::to_string(ntohs(tcph->source)), 31);
    cout << green << "|";

    centerstring(std::to_string(ntohs(tcph->dest)), 31);
    cout << green << "|" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|                       Sequence  Number                        |" << endl;
    cout << green << "|";

    centerstring(std::to_string(tcph->seq), 63);

    cout << green <<"|" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|                     Acknowledgment Number                     |" << endl;
    cout << green <<"|";

    centerstring(std::to_string(tcph->ack_seq), 63);

    cout << green <<"|" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green <<"| Offset|  Reserved |   Flags   |            Window             |" << endl;
    cout << green <<"|       |           |U|A|P|R|S|F|                               |" << endl;
    cout << green <<"|       |           |R|C|S|S|Y|I|                               |" << endl;
    cout << green <<"|       |           |G|K|H|T|N|N|                               |" << endl;
    cout << green <<"|";

    centerstring(std::to_string(tcph->doff), 7);

    cout << green << "|";

    centerstring(std::to_string(tcph->res1), 11);

    cout << green << "|";

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

    centerstring(std::to_string(tcph->window), 31);


    cout << green << "|" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|            Checksum           |         Urgent Pointer        |" << endl;
    cout << green << "|";

    centerstring(std::to_string(tcph->check), 31);

    cout << green << "|";

    centerstring(std::to_string(tcph->urg_ptr), 31);


    cout << green << "|" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|                    Options                    |    Padding    |" << endl;
    cout << green << "|                                               |               |" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;

    cout << green <<"Payload ===>  " ;

    cout << red << buff + sizeof(tcphdr) << endl;
}
