#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<linux/ip.h>
#include<linux/tcp.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int main(int argc, char const *argv[])
{
    u_int16_t port = atoi(argv[1]);
    
    // Structs that contain source IP addresses
    struct sockaddr_in source_socket_address, dest_socket_address;

    int packet_size;

    // Allocate string buffer to hold incoming packet data
    unsigned char buffer[65536];
    memset(buffer, 0, 65536);
    // Open the raw socket
    int sock = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sock == -1)
    {
        //socket creation failed, may be because of non-root privileges
        perror("Failed to create socket");
        exit(1);
    }
    while(1) {
      // recvfrom is used to read data from a socket
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

      printf("Incoming Packet: \n");
      printf("Packet Size (bytes): %d\n",ntohs(ip->tot_len));
      printf("Source Address: %s\n", (char *)inet_ntoa(source_socket_address.sin_addr));
      printf("Destination Address: %s\n", (char *)inet_ntoa(dest_socket_address.sin_addr));
      printf("Source Port: %d\n", ntohs(tcp->source));
      printf("ack_seq: %d\n", tcp->ack_seq);
      printf("res1: %d\n", tcp->res1);
      printf("fin: %d\n", tcp->fin);
      printf("Reset: %d\n", tcp->rst);
      printf("Push: %d\n", tcp->psh);
      printf("Seq: %d\n", tcp->seq);
      printf("ack: %d\n", tcp->ack);
      printf("urg: %d\n", tcp->urg);
      printf("ece: %d\n", tcp->ece);
      printf("Window: %d\n", tcp->window);

    }

    return 0;
}
