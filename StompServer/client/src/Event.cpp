#include "../include/Event.h"
#include "../include/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
using json = nlohmann::json;

using namespace std;

Event::Event(string team_a_name, string team_b_name, string name, int time,
             map<string, string> game_updates, map<string, string> team_a_updates,
             map<string, string> team_b_updates, string discription)
    : team_a_name(team_a_name), team_b_name(team_b_name), name(name),
      time(time), game_updates(game_updates), team_a_updates(team_a_updates),
      team_b_updates(team_b_updates), description(discription)
{
}

Event::~Event()
{
}

const string &Event::get_team_a_name() const
{
    return this->team_a_name;
}

const string &Event::get_team_b_name() const
{
    return this->team_b_name;
}

const string &Event::get_name() const
{
    return this->name;
}

int Event::get_time() const
{
    return this->time;
}

const map<string, string> Event::get_game_updates() const
{
    return this->game_updates;
}

const map<string, string> Event::get_team_a_updates() const
{
    return this->team_a_updates;
}

const map<string, string> Event::get_team_b_updates() const
{
    return this->team_b_updates;
}

const string &Event::get_discription() const
{
    return this->description;
}

Event::Event(const string &frame_body) : team_a_name(""), team_b_name(""), name(""), time(0), game_updates(), team_a_updates(), team_b_updates(), description("")
{
    string msg = frame_body.substr(frame_body.find("/topic/") + stringLen("/topic/"));
    team_a_name = msg.substr(0, msg.find("_"));
    msg = msg.substr(msg.find("_") +1);
    
    team_b_name = msg.substr(msg.find("_") + 1, msg.find("\n"));
    msg = msg.substr(msg.find("\nevent name: ") + stringLen("\nevent name: "));
    
    name = msg.substr(0, msg.find_first_of("\n"));
    msg = msg.substr(msg.find("\ntime: ") + stringLen("\ntime: "));    
    
    time = stoi(msg.substr(0 , msg.find("\ngeneral")));
    msg = msg.substr(msg.find("\ngeneral ") + stringLen("\ngeneral game updates:\n"));

    string toConvert = msg.substr(0, msg.find("team a"));
    game_updates = stringToMap(toConvert);
    msg = msg.substr(msg.find("\nteam a updates:") + stringLen("\nteam a updates: "));
    
    toConvert = msg.substr(0, msg.find("team b"));
    team_a_updates = stringToMap(toConvert);
    msg = msg.substr(msg.find("\nteam b updates:") + stringLen("\nteam b updates: "));
    
    toConvert = msg.substr(0, msg.find("description:"));
    team_b_updates = stringToMap(toConvert);
    msg = msg.substr(msg.find("\ndescription:") + stringLen("\ndescription:"));
    
    description = msg.substr(0, msg.length() -1);
}

int Event::stringLen(string str){
    return str.length();
}

names_and_events parseEventsFile(string json_path)
{
    ifstream f(json_path);
    json data = json::parse(f);

    string team_a_name = data["team a"];
    string team_b_name = data["team b"];

    // run over all the events and convert them to Event objects
    vector<Event> events;
    for (auto &event : data["events"])
    {
        string name = event["event name"];
        int time = event["time"];
        string description = event["description"];
        map<string, string> game_updates;
        map<string, string> team_a_updates;
        map<string, string> team_b_updates;
        for (auto &update : event["general game updates"].items())
        {
            if (update.value().is_string())
                game_updates[update.key()] = update.value();
            else
                game_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team a updates"].items())
        {
            if (update.value().is_string())
                team_a_updates[update.key()] = update.value();
            else
                team_a_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team b updates"].items())
        {
            if (update.value().is_string())
                team_b_updates[update.key()] = update.value();
            else
                team_b_updates[update.key()] = update.value().dump();
        }
        
        events.push_back(Event(team_a_name, team_b_name, name, time, game_updates, team_a_updates, team_b_updates, description));
    }
    names_and_events events_and_names{team_a_name, team_b_name, events};

    return events_and_names;
}

map<string, string> Event::stringToMap(string updatesString){
    map<string, string> updatesMap = map<string, string>();
    if(updatesString == "")
        return updatesMap;
    while(updatesString.length() > 1){
        string statName = updatesString.substr(0,updatesString.find_first_of(":") - 1);
        string statVal = updatesString.substr(updatesString.find_first_of(":") +1 ,updatesString.find_first_of("\n")- updatesString.find_first_of(":") - 1);
        updatesMap.emplace(statName, statVal);
        updatesString = updatesString.substr(updatesString.find("\n") + 1 );   
    }
    return updatesMap;
}

