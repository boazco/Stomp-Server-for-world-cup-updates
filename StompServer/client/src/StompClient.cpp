#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include <string>
#include "StompProtocol.h"
#include "Task.h"
#include <thread>

int main()
{
    while(1){
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        line = line.substr(line.find_first_of(" ") + 1, line.length() - line.find_first_of(" "));
        std::string host = line.substr(0, line.find_first_of(":"));
        line = line.substr(line.find_first_of(":") + 1);
        short port = stoi(line.substr(0, line.find_first_of(" ")));
        ConnectionHandler connectionHandler(host, port);
        if (!connectionHandler.connect()){
            std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
            return 1;
        }   
    
        line = line.substr(line.find_first_of(" ") + 1);
        string userName = line.substr(0, line.find_first_of(" "));
        line = line.substr(line.find_first_of(" ") + 1);
        string passcode = line;
    
        connectionHandler.getProtocol().setUsername(userName);
        int receiptId = connectionHandler.getProtocol().genrateReceiptId();
        bool logedIn = true;

        string msg = "CONNECT\naccept-version:1.2\nhost:" + host + "\nlogin:" + userName 
                + "\npasscode:" + passcode + "\nreceipt:" + to_string(receiptId) + "\n\n";

        if (!connectionHandler.sendFrameAscii(msg, '\0'))
        {
            std::cout << "Disconnected. Exiting... before threads\n"<< std::endl;
            logedIn = false;
        }
        
        while(logedIn){
            Task keyboard(1, logedIn);
            Task serverListener(2, logedIn);
            std::thread th1(&Task::keyBoardListener, &keyboard, std::ref(connectionHandler));
            std::thread th2(&Task::serverListener, &serverListener, std::ref(connectionHandler));
            th1.join();
            th2.join();
        }
    }
    return 0;
}
