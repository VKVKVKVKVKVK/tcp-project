#include <iostream>

using namespace std;

    int state = 0;
struct flags {
    bool syn = 0;
    bool ack = 0;
    bool fin = 0;
}

flas automate(bool syn, bool ack, bool fin, bool client){
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
        if (syn)
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
        if (ack)
        {
            state = 4;
        }
    }

    //SYN_SENT
    else if (state == 3)
    {
        cout << "SYN_SENT" << endl;
        if (syn && ack)
        {
            state = 4;
            ret.ack = true
        }
        else if (syn)
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
        if (fin)
        {
            state = 6;
            ret.ack = true;
        }   
    }

    //FIN_WAIT_1
    else if (state == 5)
    {
        cout << "FIN_WAIT_1" << endl;
        if (fin && ack)
        {
            state = 10;
            ret.ack = true;
        }
        else if (fin)
        {
            state = 7;
            ret.ack = true;
        }
        else if (ack)
        {
            state = 8;
        }
    }

    //CLOSE_WAIT
    else if (state == 6)
    {
        cout << "CLOSE_WAIT" << endl;
        ret.fin = true;
    }

    //CLOSING
    else if (state == 7)
    {
        cout << "CLOSING" << endl;
        if (ack)
        {
            state = 10;
        }
    }

    //FIN_WAIT_2
    else if (state == 8)
    {
        cout << "FIN_WAIT_2" << endl;
        if (fin)
        {
            ret.ack = true;
            state = 10;
        }
    }

    //LAST_ACK
    else if (state == 9)
    {
        cout << "LAST_ACK" << endl;
        if (ack)
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
