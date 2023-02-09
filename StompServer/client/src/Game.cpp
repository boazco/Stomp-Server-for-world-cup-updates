#include "Game.h"
#include <algorithm>

Game::Game(): generalStats(map<string, string>()), teamAstats(map<string, string>()), teamBstats(map<string,string>()), events("")
{}

void Game::addEvent(Event event){
    updateStats(event.get_game_updates(), "generalStats");
    updateStats(event.get_team_a_updates(), "teamAstats");
    updateStats(event.get_team_b_updates(), "teamBstats");
    events = events + to_string(event.get_time()) + "-" + event.get_name() + "\n\n" + event.get_discription() + "\n\n\n";
}

void Game::updateStats(map<string,string> newStat, string fieldToUse){
    if(fieldToUse == "generalStats"){
        for(std::pair<string,string> entry: newStat)
            generalStats[entry.first] = entry.second;
    }
    if(fieldToUse == "teamAstats"){
        for(std::pair<string,string> entry: newStat)
            teamAstats[entry.first] = entry.second;
    }
    
    if(fieldToUse == "teamBstats"){
        for(std::pair<string,string> entry: newStat)
            teamBstats[entry.first] = entry.second;
    }
}


string Game::lexSortStat(string teamA, string teamB){
    return "Game stats:\nGeneral stats:\n" + lexSort(generalStats) + "\n\n" + teamA + " stats:\n" + 
                lexSort(teamAstats) + "\n\n" + teamB + " stats:\n" + lexSort(teamBstats);
}

string Game::lexSort(map<string,string> stats){
    string sortedStats = ""; 
    for(std::pair<string, string> entry: stats){
        sortedStats = sortedStats + entry.first + " : " + entry.second +  "\n";
    }

    return sortedStats;
}

string Game:: getEvents(){
    return events;
}



