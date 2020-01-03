#include <zmq.h>
#include <iostream>
#include <map>

//recieves:
// 2
// 0 var
// 1 var value
//sends:
// 0 - when ready
// 0 - ping
// 0 - success writing
// -1 - error reading
// 1 value - success reading

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " portRead portSend" << std::endl;
        return 0;
    }
    std::map<std::string,int> base; //base of vars and values

    //connect to controller
    void *context = zmq_ctx_new();
    void *socketRead = zmq_socket(context, ZMQ_PULL); //for reading from controller
    void *socketSend = zmq_socket(context, ZMQ_PUSH); //for sending to controller

    //connect sockets
    std::string addrR = "tcp://localhost:";
    std::string addrS = "tcp://localhost:";
    addrR += argv[1];
    addrS += argv[2];
    zmq_connect(socketRead, addrR.c_str());
    zmq_connect(socketSend, addrS.c_str());

    //setup sockets
    int zero = 0;
    zmq_setsockopt(socketSend, ZMQ_LINGER, (void*)&zero, sizeof(zero));
    zmq_setsockopt(socketRead, ZMQ_LINGER, (void*)&zero, sizeof(zero));

    //when ready, send 0
    char ZERO = 0;
    zmq_send(socketSend, (void*)&ZERO, sizeof(ZERO), 0);

    while (true)
    {
        char type; //0 - read, 1 - write, 2 - ping
        zmq_recv(socketRead, (void*)&type, sizeof(type), 0);

        if (type == 2) //ping
        {
            //send zero as OK
            char snd = 0;
            zmq_send(socketSend, (void*)&snd, sizeof(snd), 0);
            continue;
        }

        std::string name = ""; //name of variable (sent if 0 or 1)
        char letter;

        while (true)
        {
            zmq_recv(socketRead, (void*)&letter, sizeof(letter), 0);
            if (letter == '\0') break;
            name += letter;
        }

        if (type == 1) //write
        {
            int val; //read value
            zmq_recv(socketRead, (void*)&val, sizeof(val), 0);
            base[name] = val; //set value
            //send zero as OK
            char snd = 0;
            zmq_send(socketSend, (void*)&snd, sizeof(snd), 0);
        }
        else //read
        {
            //try to find in base
            auto iter = base.find(name);
            if (iter == base.end()) //fail
            {
                //send -1 as error
                char snd = -1;
                zmq_send(socketSend, (void*)&snd, sizeof(snd), 0);
            }
            else //found
            {
                //send zero as KEEP READ
                char snd = 1;
                zmq_send(socketSend, (void*)&snd, sizeof(snd), 0);
                //send answer
                int snd2 = iter->second;
                zmq_send(socketSend, (void*)&snd2, sizeof(snd2), 0);
            }
        }
    }

    zmq_close(socketRead);
    zmq_close(socketSend);
    zmq_ctx_destroy(context);
    return 0;
}