#include <sstream>
#include <vector>
#include <thread>

#include "ctrl.h"

//questions
// 0 port id var sz ... -- ask for reading value
// 1 port id var value sz ... -- ask for writing value
// 2 port id sz ... -- ask for ping
// 3 port -- connect
// 4 port id -- add calculating node
// 5 port id -- remove calculating node
// 6 port port2 -- join with port2
// 7 port2 -- forget port2
//-where sz ... means list of visited controllers

void Potok(Controller *ctrl)
{
    ctrl->ReadInputSocket();
}

int main(int argc, char *argv[])
{
    if (argc != 2) //required argument
    {
        std::cout << "Usage: " << argv[0] << " port" << std::endl;
        return 1;
    }
    //create controller
    Controller ctrl(argv[1]);

    void *context = zmq_ctx_new();

    //create sending socket
    void *socketSend = zmq_socket(context, ZMQ_PUSH);

    int zero = 0;
    std::string addr = "tcp://localhost:";
    addr += argv[1];
    //connect sending to Controller
    zmq_connect(socketSend, addr.c_str());
    //zmq_setsockopt(socketSend, ZMQ_SNDTIMEO, (void*)&Controller::TIMEOUT, sizeof(Controller::TIMEOUT));
    zmq_setsockopt(socketSend, ZMQ_LINGER, (void*)&zero, sizeof(zero));

    //create reading socket
    void *socketRead = zmq_socket(context, ZMQ_PULL);
    zmq_bind(socketRead, "tcp://*:*");
    //zmq_setsockopt(socketRead, ZMQ_RCVTIMEO, (void*)&Controller::TIMEOUT, sizeof(Controller::TIMEOUT));
    zmq_setsockopt(socketRead, ZMQ_LINGER, (void*)&zero, sizeof(zero));
    //get port
    char portRead[1024], *pR;
    size_t len = 1024;
    zmq_getsockopt(socketRead, ZMQ_LAST_ENDPOINT, (void*)&portRead, &len);

    int dvoetochie, hp;
    for (dvoetochie = 0, hp = 1; dvoetochie < 1024; dvoetochie++) if (portRead[dvoetochie] == ':' && hp-- == 0) break;
    pR = portRead + dvoetochie + 1;

    int myPort = atoi(pR);
    //std::cout << myPort << std::endl;

    //start controller
    std::thread potok(Potok, &ctrl);
    potok.detach();

    //connect with Controller
    char msg = 3;
    zmq_send(socketSend, (void*)&msg, sizeof(msg), ZMQ_SNDMORE);
    zmq_send(socketSend, (void*)&myPort, sizeof(myPort), 0);

    //read 0 as ok
    char ans;
    zmq_recv(socketRead, (void*)&ans, sizeof(ans), 0);
    //std::cout << ('0' + ans) << std::endl;

    //MAIN CYCLE
    bool loop = true;
    while (loop)
    {
        std::cout << "> ";

        std::string input;
        std::cin >> input;

        //switch command
        if (input == "quit") loop = false; //exit
        else if (input == "create") //add node
        {
            int id, parent; std::cin >> id;
            std::getline(std::cin, input);
            //send message
            char add = 4;
            zmq_send(socketSend, (void*)&add, sizeof(add), ZMQ_SNDMORE);
            zmq_send(socketSend, (void*)&myPort, sizeof(myPort), ZMQ_SNDMORE);
            zmq_send(socketSend, (void*)&id, sizeof(id), 0);

            //get answer
            int ans;
            zmq_recv(socketRead, (void*)&ans, sizeof(ans), 0);
            if (ans == -1) std::cout << "ERROR: id already exists" << std::endl;
            else if (ans == -2) std::cout << "ERROR: cannot create node" << std::endl;
            else std::cout << "OK: " << ans << std::endl;
        }
        else if (input == "remove") //remove node
        {
            int id; std::cin >> id;

            //send message
            char rem = 5;
            zmq_send(socketSend, (void*)&rem, sizeof(rem), ZMQ_SNDMORE);
            zmq_send(socketSend, (void*)&myPort, sizeof(myPort), ZMQ_SNDMORE);
            zmq_send(socketSend, (void*)&id, sizeof(id), 0);

            //get answer
            char ans;
            zmq_recv(socketRead, (void*)&ans, sizeof(ans), 0);
            if (ans == -1) std::cout << "ERROR: id not found" << std::endl;
            else std::cout << "OK." << std::endl;
        }
        else if (input == "exec") //execute calculation on node
        {
            int id, val;
            std::string varName;
            std::cin >> id >> varName;
            std::getline(std::cin, input);

            if (!input.empty()) //write
            {
                val = atoi(input.c_str());
                char wrt = 1;
                int zero = 0;
                zmq_send(socketSend, (void*)&wrt, sizeof(wrt), ZMQ_SNDMORE);
                zmq_send(socketSend, (void*)&myPort, sizeof(myPort), ZMQ_SNDMORE);
                zmq_send(socketSend, (void*)&id, sizeof(id), ZMQ_SNDMORE);

                for (int i = 0; i < varName.size() + 1; i++) //send var name
                {
                    char c = varName.c_str()[i];
                    zmq_send(socketSend, (void*)&c, sizeof(c), ZMQ_SNDMORE);
                }
                zmq_send(socketSend, (void*)&val, sizeof(val), ZMQ_SNDMORE); //send value
                zmq_send(socketSend, (void*)&zero, sizeof(zero), 0); //empty list
                //get answer
                char status;
                zmq_recv(socketRead, (void*)&status, sizeof(status), 0);

                if (status == 0) std::cout << "OK." << std::endl; //good
                else if (status == -1) std::cout << "ERROR: id not found" << std::endl;
                else if (status == -2) std::cout << "ERROR: node is unavailable" << std::endl;
            }
            else //read
            {
                char rid = 0;
                int zero = 0;
                zmq_send(socketSend, (void*)&rid, sizeof(rid), ZMQ_SNDMORE);
                zmq_send(socketSend, (void*)&myPort, sizeof(myPort), ZMQ_SNDMORE);
                zmq_send(socketSend, (void*)&id, sizeof(id), ZMQ_SNDMORE);

                for (int i = 0; i < varName.size() + 1; i++) //send var name
                {
                    char c = varName.c_str()[i];
                    zmq_send(socketSend, (void*)&c, sizeof(c), ZMQ_SNDMORE);
                }
                zmq_send(socketSend, (void*)&zero, sizeof(zero), 0);
                //get answer
                char status; int answer;
                zmq_recv(socketRead, (void*)&status, sizeof(status), 0);
                if (status == 0) //good
                {
                    zmq_recv(socketRead, (void*)&answer, sizeof(answer), 0);
                    std::cout << "OK: " << answer << std::endl;
                }
                else if (status == -1) std::cout << "ERROR: id not found" << std::endl;
                else if (status == -2) std::cout << "ERROR: node is unavailable" << std::endl;
                else if (status == -3) std::cout << "OK: '" << varName << "' not found" << std::endl;
            }
        }
        else if (input == "ping") //ping node
        {
            int id, zero = 0; std::cin >> id;

            //send message
            char add = 2;
            zmq_send(socketSend, (void*)&add, sizeof(add), ZMQ_SNDMORE);
            zmq_send(socketSend, (void*)&myPort, sizeof(myPort), ZMQ_SNDMORE);
            zmq_send(socketSend, (void*)&id, sizeof(id), ZMQ_SNDMORE);
            zmq_send(socketSend, (void*)&zero, sizeof(zero), 0);

            //get answer
            char ans;
            zmq_recv(socketRead, (void*)&ans, sizeof(ans), 0);
            if (ans == -1) std::cout << "ERROR: id not found" << std::endl;
            else std::cout << "OK: " << char('0' + ans) << std::endl;
        }
        else if (input == "union")
        {
            int port2; std::cin >> port2;

            //send message
            char join = 6;
            zmq_send(socketSend, (void*)&join, sizeof(join), ZMQ_SNDMORE);
            zmq_send(socketSend, (void*)&myPort, sizeof(myPort), ZMQ_SNDMORE);
            zmq_send(socketSend, (void*)&port2, sizeof(port2), 0);

            //get answer
            char ans;
            zmq_recv(socketRead, (void*)&ans, sizeof(ans), 0);
            if (ans == -1) std::cout << "ERROR: cannot union self" << std::endl;
            else if (ans == -2) std::cout << "ERROR: cannot union twice" << std::endl;
            else if (ans == -3) std::cout << "ERROR: cannot union with this port" << std::endl;
            else std::cout << "OK."<< std::endl;
        }
        else if (input == "oc")
        {
            ctrl.ShowOC();
        }
        else
        {
            std::cout << "ERROR: unknown command" << std::endl;
        }
    }

    zmq_close(socketSend);
    zmq_close(socketRead);
    zmq_ctx_destroy(context);
    return 0;
}