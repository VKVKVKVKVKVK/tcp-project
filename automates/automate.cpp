#include <iostream>

using namespace std;

int main(void){
   cout << "Hello World" << endl;
   return 0;
}

void automate(void){
    int state = 0;
    //CLOSED
    if (state == 0)
    {
        cout << "CLOSED" << endl;
    }
    // LISTEN
    if (state == 1)
    {
        cout << "LISTEN" << endl;
    }
    //SYN_RECVD
    if (state == 2)
    {
        cout << "SYN_RECVD" << endl;
    }
    //SYN_SENT
    if (state == 3)
    {
        cout << "SYN_SENT" << endl;
    }
    //ESTABLISHED
    if (state == 4)
    {
        cout << "ESTABLISHED" << endl;
    }
    //FIN_WAIT_1
    if (state == 5)
    {
        cout << "FIN_WAIT_1" << endl;
    }
    //CLOSE_WAIT
    if (state == 6)
    {
        cout << "CLOSE_WAIT" << endl;
    }
    //CLOSING
    if (state == 7)
    {
        cout << "CLOSING" << endl;
    }
    //FIN_WAIT_2
    if (state == 8)
    {
        cout << "FIN_WAIT_2" << endl;
    }
    //LAST_ACK
    if (state == 9)
    {
        cout << "LAST_ACK" << endl;
    }
    //TIMED_WAIT
    if (state == 10)
    {
        cout << "TIMED_WAIT" << endl;
    }
}
