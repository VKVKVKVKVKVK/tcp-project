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

#include "../common/common.h"
#include "../automates/automate.cpp"

using namespace std;

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
    tcp->urg = 0; //
    tcp->ece = 0; //
    tcp->cwr = 0; //
    tcp->doff = 0; //
    tcp->syn = f.syn; //
    tcp->seq = 0; //
    tcp->urg_ptr = 0; //
    tcp->window = 1000; //

    tcp->check = csum((unsigned short *)buffer, sizeof(struct tcphdr));
}

int reply(int sock, flags f, in_addr_t ip, u_int16_t sport, u_int16_t dport)
{
    unsigned char buffer[PCKT_LEN]{0};
    craft_packet(buffer, f, sport, dport);
    struct sockaddr_in sin{0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(dport);
    sin.sin_addr.s_addr = ip;

    std::string synack("This is the last ACK.");
    memcpy(buffer + sizeof(struct tcphdr), &synack[0],synack.length());

    if (sendto(sock, buffer, sizeof(struct tcphdr) + synack.length(), 0,
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

  // INITIALISING AUTOMATE TO CLOSED STATE
  int state = 0;
  struct flags flag = automate(state, false, false, false, false);


  // CREATING RAW SOCKET
  sd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
  if (sd < 0) {
    perror("socket() error");
    exit(2);
  }
  printf("OK: a raw socket is created.\n");



  sin.sin_family = AF_INET;
  sin.sin_port = htons(dst_port);
  sin.sin_addr.s_addr = dst_addr;

  //////////CRAFTING TCP HEADER
  tcp->source = htons(src_port);
  tcp->dest = htons(dst_port);
  tcp->ack_seq = 0;//
  tcp->res1 = 0; //
  tcp->fin = 0; //
  tcp->rst = 0; //
  tcp->psh = 0; //
  tcp->ack = 0; //
  tcp->urg = 0; //
  tcp->ece = 0; //
  tcp->cwr = 0; //
  tcp->doff = 0; //
  tcp->syn = 1; //
  tcp->seq = 0; //
  tcp->urg_ptr = 0; //
  tcp->window = 1000; //
  tcp->check = csum((unsigned short *)buffer, sizeof(struct tcphdr));


  /////////// SENDING SYN TO SERVER//////////////////
  std::string syn("This is a SYN message.");
  memcpy(buffer + sizeof(struct tcphdr), &syn[0], syn.length());
  if (sendto(sd, buffer, sizeof(struct tcphdr) + syn.length(), 0,
             (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    perror("sendto()");
    exit(3);
  }
  printf("OK: one packet is sent.\n\n");


  /////////// INIT AUTOMAT TO SYN_SENT/////////////////
  state = 3;
  flag = automate(state, tcp->syn, tcp->ack, tcp->fin, false);


  ////////// LSITENING TO SERVER REPLY (ACK+SYN)//////
  do
  {
      memset(buffer, 0, sizeof(struct iphdr) + sizeof(struct tcphdr) + 1); //fixme: size
      socklen_t len = sizeof(sin);
      int packet_size = recvfrom(sd, buffer , 65536 , 0 , (sockaddr*)&sin, &len);
      if (packet_size == -1) {
	  printf("Failed to get packets\n");
	  return 1;
      }
  } while(((struct tcphdr*)(buffer + sizeof(struct iphdr)))->dest != htons(src_port));


  //////// INIT AUTOMAT TO SYN_RCVD///////////////////////
  printf("A packet has been received.\n");
  state = 2;
  flag = automate(state, true, true, false, false);

  print_segment((tcphdr*)(buffer + sizeof(struct iphdr)), buffer);



    //////////LAST ACK TO SERVER BEFORE ESTABLISHING CONNECTION//////////////

    struct iphdr *ip = (struct iphdr *)buffer;
    struct tcphdr *tcpbis = (struct tcphdr *)(buffer + sizeof(struct iphdr));

    state = 3;
    flag = automate(state, tcpbis->syn, tcpbis->ack, tcpbis->fin, false);
    reply(sd, flag, ip->saddr, ntohs(tcpbis->dest), ntohs(tcpbis->source));
    /////////////////////////////////////////////////////////////////////////


    state = 4;
    flag = automate(state, false, false, false, false);


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
