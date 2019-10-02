#include <ostream>
#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<linux/ip.h>
#include<linux/tcp.h>
#include<sys/socket.h>
#include<arpa/inet.h>

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

    std::string synack("This is a SYN+ACK message.");
    memcpy(buffer + sizeof(struct tcphdr), &synack[0],synack.length());

    if (sendto(sock, buffer, sizeof(struct tcphdr) + synack.length(), 0,
	       (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
	perror("sendto()");
	exit(3);
    }
    cout << "Replying ..." << endl;
    temp_sleep();

    printf("OK: one packet is sent.\n");
    return 0;
}

int main(int argc, char const *argv[])
{
    u_int16_t port = atoi(argv[1]);

    // Structs that contain source IP addresses
    struct sockaddr_in source_socket_address, dest_socket_address;

    int packet_size;

    // Allocate string buffer to hold incoming packet data
    unsigned char buffer[65536];
    //struct sockaddr_in sin;
    memset(buffer, 0, 65536);

    // INIT AUTOMAT TO CLOSED
    int state = 0;
    struct flags flag = automate(state, false, false, false, false);

    // Open the raw socket
    int sock = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sock == -1)
    {
        //socket creation failed, may be because of non-root privileges
        perror("Failed to create socket");
        exit(1);
    }

    printf("OK: a socket has been created.\n\n");

    // SET AUTOMATE TO LISTEN
    state = 1;
    flag = automate(state, false, false, false, false);

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
        //printf("Not the correct port!\n");
        ip = NULL;
        tcp = NULL;
        continue;
      }

      memset(&source_socket_address, 0, sizeof(source_socket_address));
      source_socket_address.sin_addr.s_addr = ip->saddr;
      memset(&dest_socket_address, 0, sizeof(dest_socket_address));
      dest_socket_address.sin_addr.s_addr = ip->daddr;

      // SET AUTOMAT TO SYN_RECV
      state = 2;
        if (ntohs(tcp->dest) != port){
            continue;
        } else {
            flag = automate(state, false, false, false, false);
        }

        sleep(1);
        printf("A packet has been received.\n");
      print_segment(tcp, buffer);


       state = 3;
        flag = automate(state, tcp->syn, tcp->ack, tcp->fin, false);
      reply(sock, flag, ip->saddr, ntohs(tcp->dest), ntohs(tcp->source));

        while(1) {

            unsigned char bufferbis[65536];
            //struct sockaddr_in sin;
            memset(bufferbis, 0, 65536);

            packet_size = recvfrom(sock, bufferbis, 65536, 0, NULL, NULL);
            if (packet_size == -1) {
                printf("Failed to get packets\n");
                return 1;
            }

            struct iphdr *ipbis = (struct iphdr *) bufferbis;
            struct tcphdr *tcpbis = (struct tcphdr *) (bufferbis + sizeof(struct iphdr));

            //check if dest port is equal to one of server
            if (ntohs(tcpbis->dest) != port) {
                //printf("Not the correct port!\n");
                ipbis = NULL;
                tcpbis = NULL;
                continue;
            }

            memset(&source_socket_address, 0, sizeof(source_socket_address));
            source_socket_address.sin_addr.s_addr = ipbis->saddr;
            memset(&dest_socket_address, 0, sizeof(dest_socket_address));
            dest_socket_address.sin_addr.s_addr = ipbis->daddr;

            // SET AUTOMAT TO SYN_RECV
            state = 2;
            if (ntohs(tcpbis->dest) != port) {
                continue;
            } else {
                flag = automate(state, false, false, false, false);
            }

            temp_sleep();
            printf("A packet has been received.\n");
            print_segment(tcpbis, bufferbis);

            state = 4;
            flag = automate(state, false, false, false, false);

        }
    }

    return 0;
}
