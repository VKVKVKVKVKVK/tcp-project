#include <iostream>

using namespace std;

struct flags {
    bool syn;
    bool ack;
    bool fin;

    flags()
    : syn(false), ack(false), fin(false)
    {};
};


flags automate(int state, bool syn, bool ack, bool fin, bool client) {
    Color::Modifier blue(Color::FG_BLUE);
    Color::Modifier def(Color::FG_DEFAULT);

    flags ret;

    //CLOSED
    if (state == 0)
    {
        cout << blue << "##############################################" << endl;
        cout << blue << "################   CLOSED   ##################" << endl;
        cout << blue << "##############################################" << endl;
        cout << def << endl;

        state = 1; //ouverture passive
    }

    // LISTEN
    else if (state == 1)
    {
        cout << blue << "##############################################" << endl;
        cout << blue << "################   LISTEN   ##################" << endl;
        cout << blue << "##############################################" << endl;
        cout << def << endl;

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
        cout << blue << "##############################################" << endl;
        cout << blue << "################   SYN_RCVD   ################" << endl;
        cout << blue << "##############################################" << endl;
        cout << def << endl;

        if (ack)
        {
            state = 4;
        }
    }

    //SYN_SENT
    else if (state == 3)
    {
        cout << blue << "##############################################" << endl;
        cout << blue << "################   SYN_SENT   ################" << endl;
        cout << blue << "##############################################" << endl;
        cout << def << endl;

        if (syn && ack)
        {
            state = 4;
            ret.ack = true;
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
        cout << blue << "##############################################" << endl;
        cout << blue << "##############   ESTABLISHED   ###############" << endl;
        cout << blue << "##############################################" << endl;
        cout << def << endl;

        if (fin)
        {
            state = 6;
            ret.ack = true;
        }   
    }

    //FIN_WAIT_1
    else if (state == 5)
    {
        cout << blue << "##############################################" << endl;
        cout << blue << "###############   FIN_WAIT1   ################" << endl;
        cout << blue << "##############################################" << endl;
        cout << def << endl;

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
        cout << blue << "##############################################" << endl;
        cout << blue << "##############   CLOSE_WAIT   ################" << endl;
        cout << blue << "##############################################" << endl;
        cout << def << endl;

        ret.fin = true;
    }

    //CLOSING
    else if (state == 7)
    {
        cout << blue << "##############################################" << endl;
        cout << blue << "################   CLOSING   #################" << endl;
        cout << blue << "##############################################" << endl;
        cout << def << endl;

        if (ack)
        {
            state = 10;
        }
    }

    //FIN_WAIT_2
    else if (state == 8)
    {
        cout << blue << "##############################################" << endl;
        cout << blue << "###############   FIN_WAIT2   ################" << endl;
        cout << blue << "##############################################" << endl;
        cout << def << endl;

        if (fin)
        {
            ret.ack = true;
            state = 10;
        }
    }

    //LAST_ACK
    else if (state == 9)
    {
        cout << blue << "##############################################" << endl;
        cout << blue << "################   LAST_ACK   ################" << endl;
        cout << blue << "##############################################" << endl;
        cout << def << endl;

        if (ack)
        {
            state = 0;
        }
    }

    //TIMED_WAIT
    else if (state == 10)
    {
        cout << blue << "##############################################" << endl;
        cout << blue << "###############   TIME_WAIT   ################" << endl;
        cout << blue << "##############################################" << endl;
        cout << def << endl;
        state = 0;
    }

    return ret;
}
