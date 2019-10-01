#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/tcp.h>

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

#define PCKT_LEN 8192

using namespace std;
void print_segment(tcphdr* tcph, unsigned char* buff);

unsigned short csum(unsigned short *buf, int nwords)
{
  unsigned long sum;
  for(sum=0; nwords>0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum &0xffff);
  sum += (sum >> 16);
  return (unsigned short)(~sum);
}

int main(int argc, char const *argv[])
{
  if (argc != 5) {
    printf("Error: Invalid parameters!\n");
    printf("Usage: %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);
    exit(1);
  }

  u_int16_t src_port, dst_port;
  u_int32_t src_addr, dst_addr;
  src_addr = inet_addr(argv[1]);
  dst_addr = inet_addr(argv[3]);
  src_port = atoi(argv[2]);
  dst_port = atoi(argv[4]);

  int sd;
  unsigned char buffer[PCKT_LEN];
  struct tcphdr *tcp = (struct tcphdr *)buffer;

  struct sockaddr_in sin;
  int one = 1;
  const int *val = &one;

  memset(buffer, 0, PCKT_LEN);

  // create a raw socket with TCP protocol
  sd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
  if (sd < 0) {
    perror("socket() error");
    exit(2);
  }
  printf("OK: a raw socket is created.\n");

  sin.sin_family = AF_INET;
  sin.sin_port = htons(dst_port);
  sin.sin_addr.s_addr = dst_addr;

  tcp->source = htons(src_port);
  tcp->dest = htons(dst_port);

  tcp->ack_seq = 0;//
  tcp->res1 = 0; //
  tcp->fin = 0; //
  tcp->rst = 0; //
  tcp->psh = 0; //
  tcp->ack = 0; //
  tcp->urg = 1; //
  tcp->ece = 0; //
  tcp->cwr = 0; //
  tcp->doff = 5; //
  tcp->syn = 1; //
  tcp->seq = 456; //
  tcp->urg_ptr = 0; //
  tcp->window = 1000; //



  tcp->check = csum((unsigned short *)buffer, sizeof(struct tcphdr));


  memcpy(buffer + sizeof(struct tcphdr), "coucou", sizeof("coucou"));
  if (sendto(sd, buffer, sizeof(struct tcphdr) + 6, 0,
             (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    perror("sendto()");
    exit(3);
  }
  printf("OK: one packet is sent.\n");

  ///////////////////////
  do
  {
      memset(buffer, 0, sizeof(struct iphdr) + sizeof(struct tcphdr) + 1); //fixme: size
      socklen_t len = sizeof(sin);
      int packet_size = recvfrom(sd, buffer , 65536 , 0 , (sockaddr*)&sin, &len);
      if (packet_size == -1) {
	  printf("Failed to get packets\n");
	  return 1;
      }
  } while(((struct tcphdr*)(buffer + sizeof(struct iphdr)))->dest != htons(dst_port));
  print_segment((tcphdr*)(buffer + sizeof(struct iphdr)), buffer);
  ///////////////////////

  close(sd);
  return 0;
}

/*
void ouverture_connexion(tcphdr *tcp){


    system("CLS");
    cout << "###############################################" << endl;
    cout << "#                                             #" << endl;
    cout << "#  Vous avez demander l'ouverture de TCP,     #" << endl;
    cout << "#  saisir les parametres:                     #" << endl;
    cout << "#                                             #" << endl;
    cout << "###############################################\n" << endl;



    cout << "#  Acknowledge number???                            #" << endl;
    cin >> tcp->ack_seq;

    cout << "#  Reserved Part ???                          #" << endl;
    unsigned char reserved_part;
    cin >> reserved_part;
    tcp->res1 = reserved_part;

    cout << "#  FIN ???                                    #" << endl;
    unsigned char fin;
    cin >> fin;
    tcp->fin = fin;

    cout << "#  RST ???                                    #" << endl;
    unsigned char rst;
    cin >> rst;
    tcp->rst = rst;

    cout << "#  PSH ???                                    #" << endl;
    unsigned char psh;
    cin >> psh;
    tcp->psh = psh;

    cout << "#  ACK???                                     #" << endl;
    unsigned char ack;
    cin >> ack;
    tcp->ack = ack;

    cout << "#  URG ???                                    #" << endl;
    unsigned char urg;
    cin >> urg;
    tcp->urg = urg;

    cout << "#  ECN-Echo Flag ???                          #" << endl;
    unsigned char ecn;
    cin >> ecn;
    tcp->ece = ecn;

    cout << "#  Congestion Window Reduced ???              #" << endl;
    unsigned char cwr;
    cin >> cwr;
    tcp->cwr = cwr;

    cout << "#  Data offset ???                            #" << endl;
    unsigned char data_offset;
    cin >> data_offset;
    tcp->doff = data_offset;

    cout << "#  SYN ???                                    #" << endl;
    unsigned char syn;
    cin >> syn;
    tcp->syn = syn;


    cout << "#  Sequence: ???                              #" << endl;
    cin >> tcp->seq;


    cout << "#  Urgent Pointer : ???                       #" << endl;
    cin >> tcp->urg_ptr;


    cout << "#  Window: ???                                #" << endl;
    cin >> tcp->window;

    cout << "###############################################" << endl;



    system("CLS");
}
*/

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

    cout << red << (char*)(tcph) + sizeof(tcphdr) << endl;
}
