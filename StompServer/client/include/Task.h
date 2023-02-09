#pragma once
#include "StompProtocol.h"
#include "Event.h"
#include "ConnectionHandler.h"


class Task
{
private:
    int id;
    bool& logedIn;
public:
    Task(int id, bool& logedIn);
    void keyBoardListener(ConnectionHandler& connectionHandler);
    void serverListener(ConnectionHandler& connectionHandler);
};


