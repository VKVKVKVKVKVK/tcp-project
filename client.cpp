#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#define PCKT_LEN 8192

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
  char buffer[PCKT_LEN];
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
  tcp->ack_seq = 0;
  tcp->res1 = 0;
  tcp->fin = 0;
  tcp->rst = 0;
  tcp->psh = 0;
  tcp->ack = 0;
  tcp->urg = 1;
  tcp->ece = 0;
  tcp->cwr = 0;
  tcp->doff = 5;
  tcp->syn = 1;
  tcp->seq = 456;
  tcp->urg_ptr = 0;
  tcp->window = 1000;
  tcp->check = csum((unsigned short *)buffer, sizeof(struct tcphdr));
  
  if (sendto(sd, buffer, sizeof(struct tcphdr), 0,
             (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    perror("sendto()");
    exit(3);
  }
  printf("OK: one packet is sent.\n");

  close(sd);
  return 0;
}
