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


unsigned short csum(unsigned short *buf, int nwords)
{
  unsigned long sum;
  for(sum=0; nwords>0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum &0xffff);
  sum += (sum >> 16);
  return (unsigned short)(~sum);
}


void craft_packet(unsigned char* buffer, flags f, u_int16_t src_port, u_int16_t dst_port)
{
  struct tcphdr *tcp = (struct tcphdr *)buffer;

  tcp->source = htons(src_port);
  tcp->dest = htons(dst_port);
  tcp->ack_seq = 0;//
  tcp->res1 = 0; //
  tcp->fin = f.fin; //
  tcp->rst = 0; //
  tcp->psh = 0; //
  tcp->ack = f.ack; //
  tcp->urg = 1; //
  tcp->ece = 0; //
  tcp->cwr = 0; //
  tcp->doff = 5; //
  tcp->syn = f.syn; //
  tcp->seq = 456; //
  tcp->urg_ptr = 0; //
  tcp->window = 1000; //

  tcp->check = csum((unsigned short *)buffer, sizeof(struct tcphdr));
}

int reply(int sock, flags f, in_addr_t ip, u_int16_t sport, u_int16_t dport)
{
#define PCKT_LEN 8192
    unsigned char buffer[PCKT_LEN]{0};
    craft_packet(buffer, f, sport, dport);
    struct sockaddr_in sin{0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(dport);
    sin.sin_addr.s_addr = ip;
    if (sendto(sock, buffer, sizeof(struct tcphdr), 0,
	       (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
	perror("sendto()");
	exit(3);
    }
    printf("OK: one packet is sent.\n");
    return 0;
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
    //struct sockaddr_in sin;
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
      system("sleep 1");
      reply(sock, flag, ip->saddr, ntohs(tcp->dest), ntohs(tcp->source));
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

    cout << red << (char*)(tcph) + sizeof(tcphdr) << endl;
}
