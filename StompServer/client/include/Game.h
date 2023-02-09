#pragma once
#include <string>
#include <map>
#include <vector>
#include "Event.h"

using namespace std;

class Game
{
private:
    map<string, string> generalStats;
    map<string, string> teamAstats;
    map<string, string> teamBstats;
    string events;

public:
    Game();
    void addEvent(Event event);
    void updateStats(map<string,string> newStat, string fieldToUse);
    string lexSortStat(string teamA, string teamB);
    string lexSort(map<string, string> stats);
    string getEvents();
};