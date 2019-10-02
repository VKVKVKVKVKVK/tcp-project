#include "common.h"

unsigned short csum(unsigned short *buf, int nwords)
{
  unsigned long sum;
  for(sum=0; nwords>0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum &0xffff);
  sum += (sum >> 16);
  return (unsigned short)(~sum);
}

void temp_sleep(){
    sleep(1);
    cout << "->" << endl;
    sleep(1);
    cout << "-->" << endl;
    sleep(1);
    cout << "--->|" << endl;
    sleep(1);
}

void centerstring(string s, int field)
{
    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    int l=s.length();
    int pos=(int)((field-l)/2);
    for(int i=0;i<pos;i++)
        cout<<" ";

    cout<< red <<s;
    for(int i=0;i<pos;i++)
        cout<<" ";
    if (s.length()%2 == 0) {
        cout << " ";
    }
}

void print_segment(tcphdr* tcph, unsigned char* buff)
{

    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    cout << green << " 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 " << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|        Source Port            |        Destination Port       |" << endl;
    cout << green << "|";

    centerstring(std::to_string(ntohs(tcph->source)), 31);
    cout << green << "|";

    centerstring(std::to_string(ntohs(tcph->dest)), 31);
    cout << green << "|" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|                       Sequence  Number                        |" << endl;
    cout << green << "|";

    centerstring(std::to_string(tcph->seq), 63);

    cout << green <<"|" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|                     Acknowledgment Number                     |" << endl;
    cout << green <<"|";

    centerstring(std::to_string(tcph->ack_seq), 63);

    cout << green <<"|" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green <<"| Offset|  Reserved |   Flags   |            Window             |" << endl;
    cout << green <<"|       |           |U|A|P|R|S|F|                               |" << endl;
    cout << green <<"|       |           |R|C|S|S|Y|I|                               |" << endl;
    cout << green <<"|       |           |G|K|H|T|N|N|                               |" << endl;
    cout << green <<"|";

    centerstring(std::to_string(tcph->doff), 7);

    cout << green << "|";

    centerstring(std::to_string(tcph->res1), 11);

    cout << green << "|";

    cout<< red <<to_string(tcph->urg);

    cout << green <<"|";

    cout << red << to_string(tcph->ack);

    cout << green << "|";

    cout << red <<  to_string(tcph->psh);

    cout << green << "|";

    cout << red << to_string(tcph->rst);

    cout << green <<"|";

    cout << red << to_string(tcph->syn);

    cout << green << "|";

    cout << red << to_string(tcph->fin);

    cout << green << "|";

    centerstring(std::to_string(tcph->window), 31);


    cout << green << "|" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|            Checksum           |         Urgent Pointer        |" << endl;
    cout << green << "|";

    centerstring(std::to_string(tcph->check), 31);

    cout << green << "|";

    centerstring(std::to_string(tcph->urg_ptr), 31);


    cout << green << "|" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
    cout << green << "|                    Options                    |    Padding    |" << endl;
    cout << green << "|                                               |               |" << endl;
    cout << green << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;

    cout << green <<"Payload ===>  " ;

    cout << red << (char*)(tcph) + sizeof(tcphdr) << endl;
}
