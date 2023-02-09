#include "Task.h"


Task::Task(int id, bool& logedIn):id(id), logedIn(logedIn) 
{}

void Task::keyBoardListener(ConnectionHandler& connectionHandler){
    StompProtocol& protocol = connectionHandler.getProtocol();

    while(logedIn){
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
		std::string line(buf);

        vector<string>* messgesToSend = protocol.process(line);
        for(string massage : *messgesToSend){ 
            if(massage.find("DISCONNECT")!= std::string::npos){
                logedIn = false;
            }
            if (!connectionHandler.sendFrameAscii(massage, '\0')) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                logedIn = false;
                break;
            }     
        }
    }
}

void Task::serverListener(ConnectionHandler& connectionHandler){
    StompProtocol& protocol = connectionHandler.getProtocol();
    while(logedIn){
        std::string answer;
        if (!connectionHandler.getFrameAscii(answer, '\0')) {
            std::cout << "Disconnected. Exiting... \n" << std::endl;
            logedIn = false;
            break;
        }

        answer.resize(answer.length()-1);
        if( answer.substr(0, answer.find_first_of("\n")) == "MASSAGE"){

                Event newEvent = Event(answer);
                string userName = answer.substr(answer.find("user: ") + 6, answer.find("\nreceipt:") - answer.find("user: ") - 6 );           
                string topic = newEvent.get_team_a_name() + "_" + newEvent.get_team_b_name();
                map<string, map<string, Game>>& reportsMap = protocol.getReportsMap();
                cout<< "\ntest-" + topic + "-"<<endl;
                cout<<"\ntest-" + userName + "-"<<endl;
                reportsMap[topic][userName].addEvent(newEvent);
        }

        std::cout  << answer << " " << std::endl << std::endl;
    }
}