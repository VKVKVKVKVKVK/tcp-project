#include <iostream>

using namespace std;

    int state = 0;
struct flags {
    bool syn = 0;
    bool ack = 0;
    bool fin = 0;
}

flas automate(flags f, bool client){
flags ret;

    //CLOSED
    if (state == 0)
    {
        cout << "CLOSED" << endl;
        state = 1; //ouverture passive
    }

    // LISTEN
    else if (state == 1)
    {
        cout << "LISTEN" << endl;
        if (f.syn)
        {
            state = 2;
            ret.syn =true;
            ret.ack =true;
        }

        else if (client)
        {
            state = 2;
        }
    }

    //SYN_RECVD
    else if (state == 2)
    {
        cout << "SYN_RECVD" << endl;
        if (f.ack)
        {
            state = 4;
        }
    }

    //SYN_SENT
    else if (state == 3)
    {
        cout << "SYN_SENT" << endl;
        if (f.syn && f.ack)
        {
            state = 4;
            ret.ack = true
        }
        else if (f.syn)
        {
            state = 2;
            ret.syn = true;
            ret.ack = true;
        }

    }

    //ESTABLISHED
    else if (state == 4)
    {
        cout << "ESTABLISHED" << endl;
        if (f.fin)
        {
            state = 6;
            ret.ack = true;
        }   
    }

    //FIN_WAIT_1
    else if (state == 5)
    {
        cout << "FIN_WAIT_1" << endl;
        if (f.fin && f.ack)
        {
            state = 10;
            ret.ack = true;
        }
        else if (f.fin)
        {
            state = 7;
            ret.ack = true;
        }
        else if (f.ack)
        {
            state = 8;
        }
    }

    //CLOSE_WAIT
    else if (state == 6)
    {
        cout << "CLOSE_WAIT" << endl;
    }

    //CLOSING
    else if (state == 7)
    {
        cout << "CLOSING" << endl;
        if (f.ack)
        {
            state = 10;
        }
    }

    //FIN_WAIT_2
    else if (state == 8)
    {
        cout << "FIN_WAIT_2" << endl;
        if (f.fin)
        {
            ret.ack = true;
            state = 10;
        }
    }

    //LAST_ACK
    else if (state == 9)
    {
        cout << "LAST_ACK" << endl;
        if (f.ack)
        {
            state = 0;
        }
    }

    //TIMED_WAIT
    else if (state == 10)
    {
        cout << "TIMED_WAIT" << endl;
        state = 0;
    }

    return ret;
}
