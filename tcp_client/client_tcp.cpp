#define _WINSOCK_DEPRECATED_NO_WARNINGS

//source: https://www.binarytides.com/raw-sockets-using-winsock/

#include "header.hh"

using namespace std;

int init_sock_ip(char* buf, SOCKADDR_IN& dest)
{
    std::string host("127.0.0.1");
    std::string source_ip("127.0.0.1");
    SOCKET s;

    IPV4_HDR *v4hdr = NULL;

    int optval;
    hostent *server;

    //Initialise Winsock
    WSADATA wsock;
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsock) != 0)
    {
        fprintf(stderr, "WSAStartup() failed");
        exit(EXIT_FAILURE);
    }
    printf("Initialised successfully.");
    ////////////////////////////////////////////////

    //Create Raw TCP Packet
    printf("\nCreating Raw TCP Socket...");
    if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == SOCKET_ERROR)
    {
        printf("Creation of raw socket failed.");
        return -1;
    }
    printf("Raw TCP Socket Created successfully.");
    ////////////////////////////////////////////////

    //Put Socket in RAW Mode.
    printf("\nSetting the socket in RAW mode...");
    if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char *)&optval, sizeof(optval)) == SOCKET_ERROR)
    {
        printf("failed to set socket in raw mode.");
        return -1;
    }
    printf("Successful.");
    ////////////////////////////////////////////////

    //Target Hostname
    printf("\nResolving Hostname...");
    if ((server = gethostbyname(host.c_str())) == 0)
    {
        printf("Unable to resolve.");
        return -1;
    }
    dest.sin_family = AF_INET;
    //dest.sin_port = htons(50000); //your destination port
    memcpy(&dest.sin_addr.s_addr, server->h_addr, server->h_length);
    printf("Resolved.");
    /////////////////////////////////////////////////

    v4hdr = (IPV4_HDR *)buf; //lets point to the ip header portion
    v4hdr->ip_version = 4;
    v4hdr->ip_header_len = 5;
    v4hdr->ip_tos = 0;
    v4hdr->ip_total_length = htons(sizeof(IPV4_HDR) + sizeof(TCP_HDR));
    v4hdr->ip_id = htons(2);
    v4hdr->ip_frag_offset = 0;
    v4hdr->ip_frag_offset1 = 0;
    v4hdr->ip_reserved_zero = 0;
    v4hdr->ip_dont_fragment = 1;
    v4hdr->ip_more_fragment = 0;
    v4hdr->ip_ttl = 8;
    v4hdr->ip_protocol = IPPROTO_RAW;//IPPROTO_TCP;
    v4hdr->ip_srcaddr = inet_addr(source_ip.c_str());
    v4hdr->ip_destaddr = inet_addr(inet_ntoa(dest.sin_addr));
    v4hdr->ip_checksum = 0;


    return s;
}

size_t craft_packet(char* buf)
{
    std::string payload("salut");
    IPV4_HDR* v4hdr = (IPV4_HDR*)buf;
    v4hdr->ip_total_length = htons(sizeof(IPV4_HDR) + sizeof(TCP_HDR) + payload.size());
    TCP_HDR *tcphdr = NULL;
    tcphdr = (TCP_HDR *)&buf[sizeof(IPV4_HDR)]; //get the pointer to the tcp header in the packet
    //tcp header
    tcphdr->source_port = 1234; //FIXME why htons ?
    tcphdr->dest_port = 50000; //FIXME why htons ?

    tcphdr->cwr = 0;
    tcphdr->ecn = 1;
    tcphdr->urg = 0;
    tcphdr->ack = 0;
    tcphdr->psh = 0;
    tcphdr->rst = 1;
    tcphdr->syn = 0;
    tcphdr->fin = 0;
    tcphdr->ns = 1;

    tcphdr->sequence = 123;
    tcphdr->acknowledge = 0;
    tcphdr->reserved_part1 = 0;
    tcphdr->data_offset = 0;
    tcphdr->window = 0;

    tcphdr->urgent_pointer = 0;

    tcphdr->checksum = 0; //FIXME

    char* data = &buf[sizeof(IPV4_HDR) + sizeof(TCP_HDR)];
    memcpy(data, payload.c_str(), payload.size());

    cout << "dest port: "<<tcphdr->dest_port << endl;
    cout << "source port: "<<tcphdr->source_port << endl;

    return sizeof(IPV4_HDR) + sizeof(TCP_HDR) + payload.size();
}

int send_packet(int sock, char* buf, size_t size, const SOCKADDR_IN& dest)
{
    if ((sendto(sock, buf, size, 0, (SOCKADDR *)&dest, sizeof(dest))) == SOCKET_ERROR)
    {
        printf("Error sending Packet : %d", WSAGetLastError());
        return -1;
    }
    return 0;
}

int menu_start() {

    int choix;

    cout << "###############################################" << endl;
    cout << "#                    MENU                     #" << endl;
    cout << "#                                             #" << endl;
    cout << "#  1. Ouverture de connexion                  #" << endl;
    cout << "#  2. Echange de donnees                      #" << endl;
    cout << "#  3. Fermeture de connexion                  #" << endl;
    cout << "#  4. Autres                                  #" << endl;
    cout << "#                                             #" << endl;
    cout << "###############################################" << endl;
    cout << "#  Choix ?                                    #" << endl;
    cout << "###############################################" << endl;

    cin >> choix;
    system("CLS");
    return choix;
}

void ouverture_connexion(){

    tcp_header tcp{0};
    system("CLS");
    cout << "###############################################" << endl;
    cout << "#                                             #" << endl;
    cout << "#  Vous avez demander l'ouverture de TCP,     #" << endl;
    cout << "#  saisir les parametres:                     #" << endl;
    cout << "#                                             #" << endl;
    cout << "###############################################\n" << endl;

    cout << "#  Port Source: ???                           #" << endl;
    cin >> tcp.source_port;

    cout << "#  Port Dest: ???                             #" << endl;
    cin >> tcp.dest_port;

    cout << "#  Sequence: ???                              #" << endl;
    cin >> tcp.sequence;

    cout << "#  Acknowledge ???                            #" << endl;
    cin >> tcp.acknowledge;

    cout << "#  Nonce Sum Flag: ???                        #" << endl;
    unsigned char ns;
    cin >> ns;
    tcp.ns = ns;

    cout << "#  Reserved Part ???                          #" << endl;
    unsigned char reserved_part;
    cin >> reserved_part;
    tcp.reserved_part1 = reserved_part;

    cout << "#  Data offset ???                            #" << endl;
    unsigned char data_offset;
    cin >> data_offset;
    tcp.data_offset = data_offset;

    cout << "#  FIN ???                                    #" << endl;
    unsigned char fin;
    cin >> fin;
    tcp.fin = fin;

    cout << "#  SYN ???                                    #" << endl;
    unsigned char syn;
    cin >> syn;
    tcp.syn = syn;

    cout << "#  RST ???                                    #" << endl;
    unsigned char rst;
    cin >> rst;
    tcp.rst = rst;

    cout << "#  PSH ???                                    #" << endl;
    unsigned char psh;
    cin >> psh;
    tcp.psh = psh;

    cout << "#  ACK???                                     #" << endl;
    unsigned char ack;
    cin >> ack;
    tcp.ack = ack;

    cout << "#  URG ???                                    #" << endl;
    unsigned char urg;
    cin >> urg;
    tcp.urg = urg;

    cout << "#  ECN-Echo Flag ???                          #" << endl;
    unsigned char ecn;
    cin >> ecn;
    tcp.ecn = ecn;

    cout << "#  Congestion Window Reduced ???              #" << endl;
    unsigned char cwr;
    cin >> cwr;
    tcp.cwr = cwr;

    cout << "#  Window: ???                                #" << endl;
    cin >> tcp.window;

    cout << "#  Checksum: ???                              #" << endl;
    cin >> tcp.checksum;

    cout << "#  Urgent Pointer : ???                       #" << endl;
    cin >> tcp.urgent_pointer;


    cout << "###############################################" << endl;



    system("CLS");
}

int main()
{
    /*
    int choix = menu_start();
    switch(choix) {
        case 1 :
          //  ouverture_connexion();
            break;
        case 2 :
            break;
        case 3 :
            break;
        case 4 :
            break;
    }
*/


    SOCKADDR_IN dest{ 0 };
    char buf[PACKET_MAX_SIZE];
    int sock;
    if ((sock = init_sock_ip(buf, dest)) == -1)
        "C LA MERDE";

    size_t size = craft_packet(buf);
    printf("\nSending packet...\n");
    send_packet(sock, buf, size, dest);
    std::cout << "done!" << std::endl;
    closesocket(sock);
    WSACleanup();

    return 0;
}