#pragma once

#include "Event.h"
#include "Game.h"


using namespace std;

class StompProtocol
{
private:
    int subIdCounter; 
    int receiptCounter; 
    map<string, int> topicTosubScriptionId;
    string userName;
    map<string, map<string, Game>> reportsByTopicAndUser;

public:
    StompProtocol();
    vector<string>* process(string msg);
    vector<string>* login(string msg);
    vector<string>* exit(string msg);
    vector<string>* join(string msg);
    vector<string>* report(string msg);
    vector<string>* summary(string msg);
    vector<string>* logout(string msg);
    int genrateSubId();
    int genrateReceiptId();
    Event makeEvent(string input);
    string buildMessage(Event event);
    string updatesToString(map<string,string> updates);
    map<string, map<string, Game>>& getReportsMap();
    void setUsername(string newUser);
};

        