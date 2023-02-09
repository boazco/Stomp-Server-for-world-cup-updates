#include "StompProtocol.h"
#include "exception"
#include <stdlib.h>
#include <fstream>

StompProtocol::StompProtocol(): subIdCounter(0), receiptCounter(0), topicTosubScriptionId(map<string, int>()),userName(""),
                                 reportsByTopicAndUser(map<string, map<string, Game>>())
{}

vector<string>* StompProtocol::process(string msg){
    string command = msg.substr(0, msg.find_first_of(" "));
    msg = msg.substr(msg.find_first_of(" ") + 1);
    if(command  == "login"){
        return login(msg);
    }
    if(command == "join"){
        return join(msg);
    }
    if(command == "exit"){
        return exit(msg);
    }
    if(command == "report"){
        return report(msg);
    }
    if(command == "summary"){
        return summary(msg);
    }
    if(command == "logout"){
        return logout(msg);
    }
    return new vector<string>();
}

vector<string>* StompProtocol::login(string msg){
    string host = msg.substr(0, msg.find_first_of(" "));
    msg = msg.substr(msg.find_first_of(" ")+1, msg.length() -1);
    string username = msg.substr(0,msg.find_first_of(" "));
    userName = username;
    msg = msg.substr(msg.find_first_of(" ")+1, msg.length() -1);
    string passcode = msg.substr(0,msg.find_first_of(" "));
    int receiptId = genrateReceiptId();

    vector<string>* postProcess = new vector<string>(); 
    postProcess->push_back("CONNECT\naccept-version:1.2\nhost:" + host + "\nlogin:" + username 
    + "\npasscode:" + passcode + "\nreceipt:" + to_string(receiptId) + "\n\n");
    return postProcess;

}

vector<string>* StompProtocol::join(string topic){
    int subId = genrateSubId();
    int receiptId = genrateReceiptId();
    topicTosubScriptionId.insert({topic, subId});

    vector<string>* postProcess = new vector<string>(); 
    postProcess->push_back("SUBSCRIBE\ndestination:/topic/" + topic + "\nid:" + to_string(subId) 
    + "\nreceipt:" + to_string(receiptId) + "\n\n");
    return postProcess;
}

vector<string>* StompProtocol::exit(string topic){
    int subId = topicTosubScriptionId[topic];
    std::cout << subId << std::endl;
    int receiptId = genrateReceiptId();
    topicTosubScriptionId.erase(topic);

    vector<string>* postProcess = new vector<string>(); 
    postProcess->push_back("UNSUBSCRIBE\nid:" + to_string(subId) + "\nreceipt:" + to_string(receiptId) + "\n\n");
    return postProcess;
}

vector<string>* StompProtocol::report(string filePath){
    vector<string>* postProcess = new vector<string>(); 
    try{ 
        names_and_events reportByUser = parseEventsFile(filePath);
        string topic = reportByUser.team_a_name + "_" + reportByUser.team_b_name;
        
        if(!topicTosubScriptionId.count(topic))
            return postProcess;
        
        for(Event event: reportByUser.events){
            postProcess->push_back(buildMessage(event));
        }
        return postProcess;  
    } catch(exception& e){
        cout<<"ERROR\nIlligal path."<<endl;
        return postProcess;
    }  
}

string StompProtocol::updatesToString(map<string,string> updates){
    string updatesStr = "";
    for(std::pair<string, string> entry: updates){
        updatesStr = updatesStr + entry.first + " : " + entry.second +"\n";
    }
    return updatesStr;
}

string StompProtocol::buildMessage(Event event){

    string team_a = event.get_team_a_name();
    string team_b = event.get_team_b_name();
    string topic = team_a + "_" + team_b;
    string event_name = event.get_name();
    int event_time = event.get_time();

    string general_updates = updatesToString(event.get_game_updates());
    string a_updates = updatesToString(event.get_team_a_updates());
    string b_updates = updatesToString(event.get_team_b_updates());

    string description = event.get_discription();

    int receiptId = genrateReceiptId();

    return "SEND\ndestination:/topic/" + topic + "\n\nuser: " + userName + "\nreceipt:" +  to_string(receiptId) + "\nteam a: " + team_a + "\nteam_b: " + team_b +
            "\nevent name: " + event_name + "\ntime: " + to_string(event_time) + "\ngeneral game updates:\n" + 
            general_updates + "team a updates:\n" + a_updates + "team b updates:\n" + b_updates + "description:\n" +
            description + "\n";
}

vector<string>* StompProtocol::summary(string msg){

    string team_a = msg.substr(0, msg.find_first_of("_"));
    msg = msg.substr(msg.find_first_of("_") + 1, msg.length() -1);
    string team_b = msg.substr(0, msg.find_first_of(" "));
    msg = msg.substr(msg.find_first_of(" ") + 1, msg.length() -1);
    string userName = msg.substr(0, msg.find_first_of(" "));
    string givenFile = msg.substr(msg.find_first_of(" ") + 1);
    Game game = reportsByTopicAndUser[team_a + "_" + team_b][userName];
    
    string summary = team_a + " vs " + team_b + "\n" + game.lexSortStat(team_a, team_b) + "Game event reports :" + game.getEvents();
    
    std::ofstream outfile;
    outfile.open(givenFile, std::ios::out);
    outfile << summary;
    cout<< summary <<endl;
    
    return new vector<string>();
}


vector<string>* StompProtocol::logout(string msg){
    int receiptId = genrateReceiptId();
    vector<string>* postProcess = new vector<string>(); 
    postProcess->push_back("DISCONNECT\nreceipt:" + to_string(receiptId) + "\n\n");
    return postProcess;
}

int StompProtocol::genrateSubId(){
    return subIdCounter++;
}

int StompProtocol::genrateReceiptId(){
    return receiptCounter++;
}

map<string, map<string, Game>>& StompProtocol:: getReportsMap(){
    return reportsByTopicAndUser;
}

void StompProtocol::setUsername(string newUser){
    userName = newUser;
}