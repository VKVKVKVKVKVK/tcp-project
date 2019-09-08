#include <iostream>

#include <winsock2.h>

using namespace std;

int main()
{
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;

    WSAStartup(MAKEWORD(2,0), &WSAData);
    server = socket(AF_INET, SOCK_STREAM, 0);

    string ip, port;
    cout << "Enter IP of the server:" << endl;
    cin >> ip;
    cout << "Enter Port of the server:" << endl;
    cin >> port;

    cout << "Atttempting connection to ip: " + ip + " with port: " + port + "." << endl;

    addr.sin_addr.s_addr = inet_addr(ip.c_str()); // replace the ip with your futur server ip address. If server AND client are running on the same computer, you can use the local ip 127.0.0.1
    addr.sin_family = AF_INET;
    addr.sin_port = htons(stoi(port));

    connect(server, (SOCKADDR *)&addr, sizeof(addr));
    cout << "Connected to server!" << endl;

    cout << "Write a message to the server:" << endl;
    string msg;
    cin >> msg;
    cout << msg;
    const char *m = msg.c_str();

    send(server, m, sizeof(m), 0);
    cout << "Message sent!" << endl;

    closesocket(server);
    WSACleanup();
    cout << "Socket closed." << endl << endl;
}